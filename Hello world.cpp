#include <windows.h>
#include <iostream>
#include <vector>

void ListPrinters() {
    DWORD needed, returned;
    EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, NULL, 0, &needed, &returned);

    std::vector<BYTE> buffer(needed);
    if (!EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, buffer.data(), needed, &needed, &returned)) {
        std::cerr << "EnumPrinters failed: " << GetLastError() << std::endl;
        return;
    }

    PRINTER_INFO_2* printers = (PRINTER_INFO_2*)buffer.data();
    for (DWORD i = 0; i < returned; ++i) {
        std::wcout << L"Printer Name: " << printers[i].pPrinterName << std::endl;
        std::wcout << L"Printer Port: " << printers[i].pPortName << std::endl;
        std::wcout << L"Driver Name: " << printers[i].pDriverName << std::endl;
        std::wcout << L"Status: " << printers[i].Status << std::endl;
        std::wcout << L"--------------------------" << std::endl;
    }
}

bool PrintText(const wchar_t* printerName, const wchar_t* text) {
    HANDLE hPrinter;
    DOC_INFO_1W docInfo;
    DWORD dwBytesWritten;
    DWORD dwBytesToWrite;

    // Open a handle to the printer.
    if (!OpenPrinterW(const_cast<LPWSTR>(printerName), &hPrinter, NULL)) {
        std::wcerr << L"Failed to open printer." << std::endl;
        return false;
    }

    // Fill in the document information.
    docInfo.pDocName = const_cast<LPTSTR>(TEXT("My Document"));
    docInfo.pOutputFile = NULL;
    docInfo.pDatatype = const_cast<LPTSTR>(TEXT("RAW"));

    // Start a document.
    if (StartDocPrinterW(hPrinter, 1, (LPBYTE)&docInfo) == 0) {
        std::wcerr << L"Failed to start document." << std::endl;
        ClosePrinter(hPrinter);
        return false;
    }

    // Start a page.
    if (!StartPagePrinter(hPrinter)) {
        std::wcerr << L"Failed to start page." << std::endl;
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        return false;
    }

    // Write data to the printer.
    dwBytesToWrite = (DWORD)wcslen(text) * sizeof(wchar_t);
    if (!WritePrinter(hPrinter, (LPVOID)text, dwBytesToWrite, &dwBytesWritten)) {
        std::wcerr << L"Failed to write to printer." << std::endl;
        EndPagePrinter(hPrinter);
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        return false;
    }

    // End the page.
    if (!EndPagePrinter(hPrinter)) {
        std::wcerr << L"Failed to end page." << std::endl;
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        return false;
    }

    // End the document.
    if (!EndDocPrinter(hPrinter)) {
        std::wcerr << L"Failed to end document." << std::endl;
        ClosePrinter(hPrinter);
        return false;
    }

    // Close the printer handle.
    ClosePrinter(hPrinter);

    return true;
}

int main() {
    const wchar_t* printerName = L"Adobe PDF";
    const wchar_t* text = L"Hello, Printer!";

    ListPrinters();
    // return 0;

    if (PrintText(printerName, text)) {
        std::wcout << L"Printed successfully." << std::endl;
    }
    else {
        std::wcout << L"Failed to print." << std::endl;
    }

    return 0;
}
