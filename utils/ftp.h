#include <windows.h>
#include <wininet.h>
#include <string.h>
#include <shlwapi.h>

#include "config.h"

BOOL getTempDir(char *tempPath, DWORD *pathLen)
{
    DWORD len = GetTempPath(*pathLen, tempPath);
    if (len > *pathLen)
    {
        return FALSE;
    }
    *pathLen = len;
    return TRUE;
}

BOOL fileExists(char *filePath)
{
    return PathFileExistsA(filePath);
}

char *getBasename(char *path)
{
    char *basename = strrchr(path, '\\');
    if (basename == NULL)
    {
        basename = strrchr(path, '/');
    }
    if (basename == NULL)
    {
        basename = path;
    }
    else
    {
        basename++;
    }
    return basename;
}

HINTERNET createFtpHandle(char *url, char *username, char *password)
{
    HINTERNET hInternet;

    hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0x0);
    if (!hInternet)
    {
        return NULL;
    }
    HINTERNET ftpSession = InternetConnectA(hInternet, url, INTERNET_DEFAULT_FTP_PORT, username, password, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0x0);
    if (!ftpSession)
    {
        InternetCloseHandle(hInternet);
        return NULL;
    }
    return ftpSession;
}

int downloadFileFtp(HINTERNET *ftpHandle, char *remoteFilePath, char *filePath)
{
    if (FtpGetFileA(*ftpHandle, remoteFilePath, filePath, FALSE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY, 0) == FALSE)
    {
        return 1;
    }
    return 0;
}

int uploadFileFtp(HINTERNET *ftpHandle, char *remoteFilePath, char *filePath)
{
    if (!FtpPutFileA(*ftpHandle, filePath, remoteFilePath, FTP_TRANSFER_TYPE_BINARY, 0))
    {
        return 1;
    }
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------
// Alternate implementations
int uploadFileFTP(const char *url, const char *username, const char *password, const char *filePath, const char *remote_file_name)
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

int downloadFTPFile(const char *url, const char *local_path)
{
    HINTERNET hInternet = NULL, hFtpSession = NULL, hFile = NULL;
    DWORD bytesRead = 0, bytesWritten = 0;
    int success = 0;

    // Open an Internet session
    hInternet = InternetOpenA("FTP Downloader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL)
    {
        success = 1;
        goto cleanup;
    }

    // Connect to the FTP server
    hFtpSession = InternetConnectA(hInternet, url, INTERNET_DEFAULT_FTP_PORT, FTP_USER, FTP_PASS, INTERNET_SERVICE_FTP, 0, 0);
    if (hFtpSession == NULL)
    {
        success = 2;
        goto cleanup;
    }

    // Open the file on the FTP server for reading
    hFile = FtpOpenFileA(hFtpSession, url, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);
    if (hFile == NULL)
    {
        success = 3;
        goto cleanup;
    }

    // Open the local file for writing
    HANDLE hLocalFile = CreateFileA(local_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hLocalFile == INVALID_HANDLE_VALUE)
    {
        success = 4;
        goto cleanup;
    }

    // Read the file from the FTP server and write it to the local file
    while (InternetReadFile(hFile, &bytesRead, sizeof(bytesRead), &bytesWritten))
    {
        if (bytesRead == 0)
        {
            break;
        }
        WriteFile(hLocalFile, &bytesWritten, bytesRead, &bytesWritten, NULL);
    }

cleanup:
    // Close all handles and free resources
    if (hFile != NULL)
    {
        InternetCloseHandle(hFile);
    }
    if (hFtpSession != NULL)
    {
        InternetCloseHandle(hFtpSession);
    }
    if (hInternet != NULL)
    {
        InternetCloseHandle(hInternet);
    }
    return success;
}
