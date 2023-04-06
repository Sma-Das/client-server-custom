#include <windows.h>
#include <wininet.h>

void createFtpHandle(HINTERNET *ftpSession, char *url, char *username, char *password)
{
    HINTERNET hInternet;

    hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0x0);
    if (!hInternet)
    {
        *ftpSession = NULL;
        return;
    }
    *ftpSession = InternetConnectA(hInternet, url, INTERNET_DEFAULT_FTP_PORT, username, password, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0x0);
    if (!(*ftpSession))
    {
        *ftpSession = NULL;
        InternetCloseHandle(hInternet);
        return;
    }
}

int downloadFileFtp(HINTERNET *ftpHandle, char *remoteFilePath, char *filePath)
{
    BOOL result;

    // Set passive mode
    // FtpSetOption(hFtpSession, INTERNET_OPTION_PASSIVE, NULL, 0);

    // Download file from FTP server
    result = FtpGetFileA(*ftpHandle, remoteFilePath, filePath, FALSE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY, 0);
    if (!result)
    {
        InternetCloseHandle(ftpHandle);
        return 3; // Error downloading file
    }

    // Cleanup resources
    InternetCloseHandle(ftpHandle);

    return 0; // Success
}

int uploadFileFtp(const char *url, const char *username, const char *password, const char *filePath, const char *remote_file_name)
{
    HINTERNET hInternet, hFtpSession;
    BOOL result;

    // Open internet session
    hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
    {
        return 1; // Error opening internet session
    }

    // Connect to FTP server
    hFtpSession = InternetConnectA(hInternet, url, INTERNET_DEFAULT_FTP_PORT, username, password, INTERNET_SERVICE_FTP, 0, 0);
    if (!hFtpSession)
    {
        InternetCloseHandle(hInternet);
        return 2; // Error connecting to FTP server
    }

    // Upload file to FTP server
    result = FtpPutFileA(hFtpSession, filePath, remote_file_name, FTP_TRANSFER_TYPE_BINARY, 0);
    if (!result)
    {
        InternetCloseHandle(hFtpSession);
        InternetCloseHandle(hInternet);
        return 3; // Error uploading file
    }

    // Cleanup resources
    InternetCloseHandle(hFtpSession);
    InternetCloseHandle(hInternet);

    return 0; // Success
}
