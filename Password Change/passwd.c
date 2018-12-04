#include <windows.h>   
#include <lm.h>   
#include <stdio.h>   
#include <string.h>   
#include <conio.h>   
   
#define MAXNAMELEN 200   
#define MAXPASSWDLEN 50   
   
#define RETVAL_SUCCESS 0   
#define RETVAL_INVALID_CMDLINE 1   
#define RETVAL_BREAK 2   
#define RETVAL_VERIFY_ERROR 3   
#define RETVAL_CHANGE_ERROR 4   
   
#define CH_BACKSPACE  8   
#define CH_ENTER     13   
#define CH_BLANK     32   
#define CH_ASTERISK  42   
   
#define AnsiToUnicode(FROM,TO)           \   
    MultiByteToWideChar(CP_ACP,          \   
                        0,               \   
                        FROM,            \   
                        strlen(FROM)+1,  \   
                        TO,              \   
                        sizeof(TO))   
   
#define OemToUnicode(FROM,TO)            \   
    MultiByteToWideChar(CP_OEMCP,        \   
                        0,               \   
                        FROM,            \   
                        strlen(FROM)+1,  \   
                        TO,              \   
                        sizeof(TO))   
   
UCHAR szAccountName[MAXNAMELEN];   
UCHAR szDomainName[MAXNAMELEN];   
UCHAR szSpecifiedAccountName[MAXNAMELEN];   
UCHAR szSpecifiedDomainName[MAXNAMELEN];   
UCHAR szOldPassword[MAXPASSWDLEN];   
UCHAR szNewPassword[MAXPASSWDLEN];   
UCHAR szVerification[MAXPASSWDLEN];   
   
WCHAR wszAccountName[MAXNAMELEN];   
WCHAR wszDomainName[MAXNAMELEN];   
WCHAR wszOldPassword[MAXPASSWDLEN];   
WCHAR wszNewPassword[MAXPASSWDLEN];   
   
BOOL bQuiet=FALSE;   
BOOL bInfo=FALSE;   
BOOL bPasswordsSpecified=FALSE;   
BOOL bUserSpecified=FALSE;   
BOOL bDomainSpecified=FALSE;   
   
void Usage(char *cmdname)   
{   
    printf("Usage: %s [-q] [-i] [-p oldpass newpass] [-u user] [-d domain]\n",cmdname);   
    printf("       change password on the command line\n");   
    printf("       -q (quiet)     suppress display of * while entering passwords\n");   
    printf("       -i (info)      print account and domain information\n");   
    printf("       -p (passwords) specify old and new password on the command line\n");   
    printf("       -u (user)      change password for any other user (default: you)\n");   
    printf("       -d (domain)    select a different domain/machine (default: your domain)\n");   
    printf("       (C) 1997 by Alexander Frink (Alexander.Frink@Uni-Mainz.DE)\n");   
    printf("                All rights reserved.\n");   
    exit(RETVAL_INVALID_CMDLINE);   
}   
   
void ProcessCmdLine(int argc, char *argv[])   
{   
    char c, *p, *arg0;   
   
    // algorithm for command line processing taken from SDK sample    
    // Win32/WinNT/Floppy/MFMT.C   
   
    arg0=argv[0];   
   
    if ( argc > 1 ) {    
        while (--argc > 0 ) {    
            p = *++argv;    
            if (*p == '/' || *p == '-') {    
                while (c = *++p) {   
                    switch (tolower(c)) {    
                    case '?':    
                        Usage(arg0);   
                        break;    
                    case 'q':    
                        bQuiet = TRUE;    
                        break;    
                    case 'i':    
                        bInfo = TRUE;   
                        break;   
                    case 'p':    
                        if (argc<3) Usage(arg0);   
                        bPasswordsSpecified = TRUE;   
                        argc--, argv++;    
                        strcpy(szOldPassword,*argv);    
                        argc--, argv++;    
                        strcpy(szNewPassword,*argv);    
                        break;   
                    case 'u':    
                        if (argc<2) Usage(arg0);   
                        bUserSpecified = TRUE;   
                        argc--, argv++;    
                        strcpy(szSpecifiedAccountName,*argv);    
                        break;   
                    case 'd':    
                        if (argc<2) Usage(arg0);   
                        bDomainSpecified = TRUE;   
                        argc--, argv++;    
                        strcpy(szSpecifiedDomainName,*argv);    
                        break;   
                    default:    
                        Usage(arg0);    
                        break;    
                    }    
                }    
            }   
        }    
    }    
    
}   
   
void GetUserAndDomain(UCHAR *szAccountName, UCHAR *szDomainName)   
{   
    HANDLE hProcess, hAccessToken;   
    UCHAR InfoBuffer[1000];   
    PTOKEN_USER pTokenUser = (PTOKEN_USER)InfoBuffer;   
    DWORD dwInfoBufferSize, dwAccountSize = MAXNAMELEN, dwDomainSize = MAXNAMELEN;   
    SID_NAME_USE snu;   
   
    hProcess = GetCurrentProcess();   
   
    OpenProcessToken(hProcess,TOKEN_READ,&hAccessToken);   
   
    GetTokenInformation(hAccessToken,   
                        TokenUser,   
                        InfoBuffer,   
                        1000,   
                        &dwInfoBufferSize);   
   
    LookupAccountSid(NULL,   
                     pTokenUser->User.Sid,   
                     szAccountName,   
                     &dwAccountSize,   
                     szDomainName,   
                     &dwDomainSize,   
                     &snu);   
}   
   
BOOL GetPassword(UCHAR *szPrompt,UCHAR *szPasswd)   
{   
/*  
    this subroutine is partially (C) by  
    Steffen Krause: skrause@informatik.hu-berlin.de,  
    the author of SuSrv, a SU utility for Windows NT  
    Improvements: allow correction of typing errors (with 'Backspace')  
                  exit on CTRL-C (returns FALSE, otherwise TRUE)  
                  skip non-printable characters (function/cursor/escape)  
                  optional: suppress *'s while entering password  
*/   
       
    int i;   
    int ch;   
    i = 0;   
    printf("%s",szPrompt);   
    while(((ch = _getch()) != CH_ENTER) && (i < MAXPASSWDLEN))   
    {   
        if (ch==CH_BACKSPACE) {   
            if (i>0) {   
                szPasswd[i]=0;   
                i--;   
                if (!bQuiet) {   
                    _putch(CH_BACKSPACE);   
                    _putch(CH_BLANK);   
                    _putch(CH_BACKSPACE);   
                }   
            }   
        } else if (ch==3) {   
            szPasswd[0]=0;   
            return FALSE;   
        } else if ((ch==0)||(ch==224)) {   
            // probably function or cursor key, skip this and next character   
            ch=_getch();   
        } else if (ch==27) {   
            // escape key, skip   
        } else {   
            szPasswd[i++] = ch;   
            if (!bQuiet) {   
                _putch(CH_ASTERISK);   
            }   
        }   
    }   
    szPasswd[i] = 0;   
    return TRUE;   
}   
   
void PrintErrorMessage(NET_API_STATUS status)   
{   
    /*  
    In contrast to the documentation on NetUserChangePassword  
    (SDK 4.0 and VC++ 4.2), other error codes in addition to the listed  
    NET_API_STATUS are returned. Therefore I decided to list all possible  
    errors from winerror.h which have to do with passwords.  
    */   
       
    switch (status) {   
    case NERR_Success:   
        // nothing to do   
        break;   
    case ERROR_ACCESS_DENIED:   
        printf("Error: The user does not have access to the requested information.\n");   
        break;   
    case NERR_InvalidComputer:   
        printf("Error: The computer name is invalid.\n");   
        break;   
    case NERR_NotPrimary:   
        printf("Error: The operation is allowed only on the primary domain controller of the domain.\n");   
        break;   
    case NERR_UserNotFound:   
        printf("Error: The user name could not be found.\n");   
        break;   
    case NERR_PasswordTooShort:   
        printf("Error: The password is shorter than required.\n");   
        break;   
    case ERROR_INVALID_PASSWORD:   
        printf("Error: The specified network password is not correct.\n");   
        break;   
    case ERROR_INVALID_PASSWORDNAME:   
        printf("Error: The format of the specified password is invalid.\n");   
        break;   
    case ERROR_NULL_LM_PASSWORD:   
        printf("Error: The NT password is too complex to be converted to a LAN Manager password.\n");   
        break;   
    case ERROR_WRONG_PASSWORD:   
        printf("Error: Unable to update the password. The value provided as the current password is incorrect.\n");   
        break;   
    case ERROR_ILL_FORMED_PASSWORD:   
        printf("Error: Unable to update the password. The value provided for the new password contains values that are not allowed in passwords.\n");   
        break;   
    case ERROR_PASSWORD_RESTRICTION:   
        printf("Error: Unable to update the password because a password update rule has been violated.\n");   
        break;   
    case ERROR_LOGON_FAILURE:   
        printf("Error: Logon failure: unknown user name or bad password.\n");   
        break;   
    case ERROR_PASSWORD_EXPIRED:   
        printf("Error: Logon failure: the specified account password has expired.\n");   
        break;   
    case ERROR_NT_CROSS_ENCRYPTION_REQUIRED:   
        printf("Error: A cross-encrypted password is necessary to change a user password.\n");   
        break;   
    case ERROR_LM_CROSS_ENCRYPTION_REQUIRED:   
        printf("Error: A cross-encrypted password is necessary to change this user password.\n");   
        break;   
    case ERROR_NO_SUCH_DOMAIN:   
        printf("Error: The specified domain did not exist.\n");   
        break;   
    default:   
        printf("Error: Undocumented error code %i.",status);   
    }   
}   
   
int main(int argc, char *argv[])   
{   
    NET_API_STATUS status;   
   
    ProcessCmdLine(argc, argv);   
   
    GetUserAndDomain(szAccountName,szDomainName);   
    if (bUserSpecified) {   
        strcpy(szAccountName,szSpecifiedAccountName);   
    }   
    if (bDomainSpecified) {   
        strcpy(szDomainName,szSpecifiedDomainName);   
    }   
   
    if (bInfo) {   
        printf("Changing password for user '%s' on domain or machine '%s'...\n",szAccountName,szDomainName);   
    }   
   
    if (!bPasswordsSpecified) {   
        if (!GetPassword("Old password: ",szOldPassword)) {   
            return RETVAL_BREAK;   
        }   
        if (!GetPassword("\nNew password: ",szNewPassword)) {   
            return RETVAL_BREAK;   
        }   
        if (!GetPassword("\nVerification: ",szVerification)) {   
            return RETVAL_BREAK;   
        }   
        printf("\n");   
   
        if (strcmp(szNewPassword,szVerification)!=0) {   
            printf("Password verification failed!\n");   
            return RETVAL_VERIFY_ERROR;   
        }   
    }   
   
    AnsiToUnicode(szAccountName,wszAccountName);   
    AnsiToUnicode(szDomainName,wszDomainName);   
    OemToUnicode(szOldPassword,wszOldPassword);   
    OemToUnicode(szNewPassword,wszNewPassword);   
       
    status=NetUserChangePassword(wszDomainName,   
                                 wszAccountName,   
                                 wszOldPassword,   
                                 wszNewPassword);   
    if (status==NERR_Success) {   
        return RETVAL_SUCCESS;   
    } else {   
        PrintErrorMessage(status);   
        return RETVAL_CHANGE_ERROR;   
    }   
}   

