

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

void PrintDocument(const char* printerName, const char* documentData) {
    HANDLE hPrinter;
    DOC_INFO_1 docInfo;
    DWORD dwBytesWritten;

    // Open the printer.
    if (!OpenPrinter((LPWSTR)printerName, &hPrinter, NULL)) {
        std::cerr << "Failed to open printer: " << GetLastError() << std::endl;
        return;
    }

    // Initialize the document info.
    docInfo.pDocName = (LPWSTR)"My Document";
    docInfo.pOutputFile = NULL;
    docInfo.pDatatype = (LPWSTR)"RAW";

    // Start the document.
    if (StartDocPrinter(hPrinter, 1, (LPBYTE)&docInfo) == 0) {
        std::cerr << "Failed to start document: " << GetLastError() << std::endl;
        ClosePrinter(hPrinter);
        return;
    }

    // Start the page.
    if (StartPagePrinter(hPrinter) == 0) {
        std::cerr << "Failed to start page: " << GetLastError() << std::endl;
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        return;
    }

    // Write the document data to the printer.
    if (!WritePrinter(hPrinter, (LPVOID)documentData, strlen(documentData), &dwBytesWritten)) {
        std::cerr << "Failed to write to printer: " << GetLastError() << std::endl;
    }

    // End the page.
    if (EndPagePrinter(hPrinter) == 0) {
        std::cerr << "Failed to end page: " << GetLastError() << std::endl;
    }

    // End the document.
    if (EndDocPrinter(hPrinter) == 0) {
        std::cerr << "Failed to end document: " << GetLastError() << std::endl;
    }

    // Close the printer.
    ClosePrinter(hPrinter);
}

int main() {
    const char* printerName = "YourPrinterName";
    const char* documentData = "This is a test document.";

    PrintDocument(printerName, documentData);
    ListPrinters();
    return 0;
}