#include <windows.h>
#include <wininet.h>

int downloadFileFtp(const char *url, const char *username, const char *password, const char *local_file_path)
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
    hFtpSession = InternetConnect(hInternet, url, INTERNET_DEFAULT_FTP_PORT, username, password, INTERNET_SERVICE_FTP, 0, 0);
    if (!hFtpSession)
    {
        InternetCloseHandle(hInternet);
        return 2; // Error connecting to FTP server
    }

    // Set passive mode
    // FtpSetOption(hFtpSession, INTERNET_OPTION_PASSIVE, NULL, 0);

    // Download file from FTP server
    result = FtpGetFileA(hFtpSession, url, local_file_path, FALSE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY, 0);
    if (!result)
    {
        InternetCloseHandle(hFtpSession);
        InternetCloseHandle(hInternet);
        return 3; // Error downloading file
    }

    // Cleanup resources
    InternetCloseHandle(hFtpSession);
    InternetCloseHandle(hInternet);

    return 0; // Success
}

int uploadFileFtp(const char *url, const char *username, const char *password, const char *local_file_path, const char *remote_file_name)
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
    hFtpSession = InternetConnect(hInternet, url, INTERNET_DEFAULT_FTP_PORT, username, password, INTERNET_SERVICE_FTP, 0, 0);
    if (!hFtpSession)
    {
        InternetCloseHandle(hInternet);
        return 2; // Error connecting to FTP server
    }

    // Upload file to FTP server
    result = FtpPutFileA(hFtpSession, local_file_path, remote_file_name, FTP_TRANSFER_TYPE_BINARY, 0);
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
