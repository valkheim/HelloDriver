#include <ntddk.h>


EXTERN_C_START
DRIVER_INITIALIZE DriverEntry;
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);
EXTERN_C_END

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#endif

static UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDriver");
static UNICODE_STRING SymLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDriver");
static PDEVICE_OBJECT DeviceObject = nullptr;

void Unload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    IoDeleteSymbolicLink(&SymLinkName);
    IoDeleteDevice(DeviceObject);
    DbgPrint("Driver unloaded\n");
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    PAGED_CODE();
    auto status = STATUS_SUCCESS;

    status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
    if (!NT_SUCCESS(status))
    {
        DbgPrint("Failed to create device\n");
        return status;
    }

    status = IoCreateSymbolicLink(&SymLinkName, &DeviceName);
    if (!NT_SUCCESS(status))
    {
        DbgPrint("Failed to create symlink\n");
        IoDeleteDevice(DeviceObject);
        return status;
    }

    DriverObject->DriverUnload = Unload;
    DbgPrint("Driver loaded\n");
    return status;
}
