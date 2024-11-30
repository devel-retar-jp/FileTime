/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Windows専用 Win32
/// ファイルの時間を表示するプログラム
/// 
/// Creation time:    作成日時
/// Last access time: 最後にアクセスした時間
/// Last write time:  最後に書き換えられた時間
/// 
/// > FileTImes.cpp
/// 
///   履歴	
///   2024/11/30	V1.00 Build Up
///   2024/11/30	V1.00 First
///
///
///                        Copyright(c) 2024, Retar.jp, All Rights Reserved.
///                        http://www.retar.jp/
/// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <shlwapi.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// リンクするライブラリ
#pragma comment(lib, "Shlwapi.lib")
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GetFilesInDirectory(const std::wstring& directory, std::vector<std::wstring>& files);
void PrintFileTime(const FILETIME& ft);
void GetFileTimes(const std::wstring& filePath);
std::string ConvertWstringToShiftJis(const std::wstring& wstr);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ファイルの時間
void PrintFileTime(const FILETIME& ft) 
{
    SYSTEMTIME st;
    FileTimeToSystemTime(&ft, &st);
    std::cout << st.wYear << "-" << st.wMonth << "-" << st.wDay << " "
        << st.wHour << ":" << st.wMinute << ":" << st.wSecond << std::endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ファイルのアクセスした時間
void GetFileTimes(const std::wstring& filePath)
{
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Could not open file" << std::endl;
        return;
    }
    //
    FILETIME creationTime, lastAccessTime, lastWriteTime;
    if (GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime)) 
    {
        std::cout << "Creation time:   \t";
        PrintFileTime(creationTime);
        std::cout << "Last write time: \t";
        PrintFileTime(lastWriteTime);
        std::cout << "Last access time:\t";
        PrintFileTime(lastAccessTime);
    }
    else {
        std::cerr << "Could not get file times" << std::endl;
    }
    //
    CloseHandle(hFile);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GetFilesInDirectory(const std::wstring& directory, std::vector<std::wstring>& files)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((directory + L"\\*").c_str(), &findFileData);
    //
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        std::cerr << "Error finding files in directory" << std::endl;
        return;
    }
    //
    do {
        const std::wstring fileOrDirName = findFileData.cFileName;
        // "." と ".." をスキップ
        if (fileOrDirName != L"." && fileOrDirName != L"..")
        {
            // ファイルの場合、リストに追加
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
            {
                files.push_back(directory + L"\\" + fileOrDirName);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    //
    FindClose(hFind);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// std::wstring を Shift_JIS にエンコードされた std::string に変換する関数
std::string ConvertWstringToShiftJis(const std::wstring& wstr) 
{
    if (wstr.empty()) {
        return std::string();
    }
    //
    // 必要なバッファサイズを取得
    int size_needed = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    if (size_needed <= 0) {
        return std::string();
    }
    //
    // バッファに変換結果を格納
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    //
    return str;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    // ディレクトリのパスを指定
    std::wstring directory = L"\\\\h410\\share\\ShinNeAshi-TimeKeep";
    //std::wstring directory = L"C:\\share\\stock";
    std::vector<std::wstring> files;
    //
    GetFilesInDirectory(directory, files);
    //
    std::wcout << "----------------------------------------" << std::endl;
    std::wcout << L"Files in directory " << directory << L":" << std::endl;
    std::wcout << "----------------------------------------" << std::endl;
    //
    for (const auto& fileFullPath : files)
    {
        std::wstring fname = PathFindFileName(fileFullPath.c_str());
        std::cout << ConvertWstringToShiftJis(fname) << std::endl;
        GetFileTimes(fileFullPath);
        std::wcout << "----------------------------------------" << std::endl;
    }
    //
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
