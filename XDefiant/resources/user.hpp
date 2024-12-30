#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

bool initWMI(IWbemLocator** pLocator, IWbemServices** pServices) {
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM library. Error code = 0x" << std::hex << hr << std::endl;
        return false;
    }

    hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize security. Error code = 0x" << std::hex << hr << std::endl;
        CoUninitialize();
        return false;
    }

    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)pLocator);
    if (FAILED(hr)) {
        std::cerr << "Failed to create IWbemLocator object. Error code = 0x" << std::hex << hr << std::endl;
        CoUninitialize();
        return false;
    }

    hr = (*pLocator)->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, pServices);
    if (FAILED(hr)) {
        std::cerr << "Could not connect to WMI server. Error code = 0x" << std::hex << hr << std::endl;
        (*pLocator)->Release();
        CoUninitialize();
        return false;
    }

    return true;
}

bool checkMotherboardSerial(const std::string& expectedSerialNumber) {
    IWbemLocator* pLocator = NULL;
    IWbemServices* pServices = NULL;
    IEnumWbemClassObject* pEnumerator = NULL;

    if (!initWMI(&pLocator, &pServices)) {
        return false;
    }

    HRESULT hr = pServices->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_BaseBoard"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

    if (FAILED(hr)) {
        std::cerr << "Query for motherboard information failed. Error code = 0x" << std::hex << hr << std::endl;
        pServices->Release();
        pLocator->Release();
        CoUninitialize();
        return false;
    }

    IWbemClassObject* pClassObject = NULL;
    ULONG uReturn = 0;
    bool result = false;

    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pClassObject, &uReturn);
        if (0 == uReturn) {
            break;
        }

        VARIANT vtProp;
        hr = pClassObject->Get(L"SerialNumber", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr)) {
            // Convert _bstr_t to std::string correctly
            _bstr_t bstrSerialNumber(vtProp.bstrVal);
            std::string serialNumber = static_cast<const char*>(bstrSerialNumber);

            if (serialNumber == expectedSerialNumber) {
                result = true;
            }
            VariantClear(&vtProp);
        }

        pClassObject->Release();
    }

    pEnumerator->Release();
    pServices->Release();
    pLocator->Release();
    CoUninitialize();

    return result;
}