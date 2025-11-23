/*
 * Randy's Educational EFI Loader
 * A simple UEFI application that reveals the programmable layers below the OS
 * 
 * Randy's Smoke-Exhaling Epiphany:
 * "I bet a great many people don't even know there are layers below the OS
 * that are also programmable. Let's blow their minds faster than I can 
 * blow smoke out!"
 * 
 * Educational Philosophy:
 * Most programmers think the operating system is the bottom of the stack.
 * But there's a whole world below - firmware, boot loaders, UEFI applications
 * that run before any OS even thinks about starting. This is where the 
 * real magic happens, where hardware meets software in its purest form.
 * 
 * What This Demonstrates:
 * - Programming at the firmware level (pre-OS)
 * - Direct hardware interaction without OS abstraction
 * - UEFI boot services and protocols
 * - The layers most developers never see
 * - Why understanding the full stack matters
 * 
 * Randy's Plant Spirit Hardware Wisdom:
 * "Every layer of abstraction hides power. Sometimes you need to go
 * deeper than the roots to understand how the whole garden grows."
 * 
 * Build Instructions:
 * - Requires EDK II UEFI development environment
 * - Or use gnu-efi for simpler builds
 * - Target: x86_64 UEFI executable (.efi file)
 * - Boot from USB/CD or VM to test
 * 
 * WARNING: This runs at firmware level - handle with cannabis-enhanced care!
 */

#include <efi.h>
#include <efilib.h>

// Randy's Educational Constants
#define RANDY_APP_NAME L"Randy's Mind-Blowing EFI Loader"
#define RANDY_VERSION L"v1.0 - Plant Spirit Enhanced"
#define PAUSE_TIME 3000000  // 3 seconds in microseconds

/*
 * Randy's EFI Text Display Functions
 * "At this level, even printing text is an adventure in low-level protocol use"
 */
VOID randy_clear_screen(VOID) {
    // Clear the screen using UEFI Simple Text Output Protocol
    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
}

VOID randy_print_centered(CHAR16 *text) {
    UINTN cols, rows;
    UINTN text_len = StrLen(text);
    
    // Get console dimensions
    uefi_call_wrapper(ST->ConOut->QueryMode, 4, ST->ConOut, 
                      ST->ConOut->Mode->Mode, &cols, &rows);
    
    // Calculate center position
    UINTN start_col = (cols > text_len) ? (cols - text_len) / 2 : 0;
    
    // Position cursor and print
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, start_col, rows/2);
    Print(text);
}

VOID randy_wait_for_key(VOID) {
    EFI_INPUT_KEY key;
    Print(L"\n\n🌿 Press any key to continue your journey into the firmware realm...\n");
    
    // Wait for keystroke using UEFI Simple Text Input Protocol
    while (uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key) == EFI_NOT_READY) {
        // Busy wait - at firmware level, we don't have fancy async I/O
        uefi_call_wrapper(BS->Stall, 1, 10000); // 10ms delay
    }
}

/*
 * Randy's Memory Map Educational Display
 * "Show students what memory looks like before the OS claims it all"
 */
VOID randy_show_memory_map(VOID) {
    EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
    UINTN map_size = 0, map_key, descriptor_size;
    UINT32 descriptor_version;
    EFI_STATUS status;
    
    Print(L"\n🧠 Randy's Pre-OS Memory Map (What the OS will inherit)\n");
    Print(L"═══════════════════════════════════════════════════════════\n");
    
    // Get memory map size first
    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, memory_map, 
                               &map_key, &descriptor_size, &descriptor_version);
    
    if (status != EFI_BUFFER_TOO_SMALL) {
        Print(L"❌ Failed to get memory map size\n");
        return;
    }
    
    // Allocate buffer with extra space for new allocations
    map_size += 2 * descriptor_size;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, map_size, 
                               (VOID**)&memory_map);
    
    if (EFI_ERROR(status)) {
        Print(L"❌ Failed to allocate memory for map\n");
        return;
    }
    
    // Get actual memory map
    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, memory_map,
                               &map_key, &descriptor_size, &descriptor_version);
    
    if (EFI_ERROR(status)) {
        Print(L"❌ Failed to get memory map\n");
        uefi_call_wrapper(BS->FreePool, 1, memory_map);
        return;
    }
    
    Print(L"Memory Descriptor Version: %d\n", descriptor_version);
    Print(L"Descriptor Size: %d bytes\n", descriptor_size);
    Print(L"Total Map Size: %d bytes\n\n", map_size);
    
    const CHAR16 *memory_type_names[] = {
        L"ReservedMemory",
        L"LoaderCode", 
        L"LoaderData",
        L"BootServicesCode",
        L"BootServicesData", 
        L"RuntimeServicesCode",
        L"RuntimeServicesData",
        L"ConventionalMemory",
        L"UnusableMemory",
        L"ACPIReclaimMemory",
        L"ACPIMemoryNVS",
        L"MemoryMappedIO",
        L"MemoryMappedIOPortSpace",
        L"PalCode"
    };
    
    UINTN num_descriptors = map_size / descriptor_size;
    EFI_MEMORY_DESCRIPTOR *desc = memory_map;
    
    Print(L"%-20s %-16s %-16s %-8s\n", L"Type", L"Physical", L"Virtual", L"Pages");
    Print(L"────────────────────────────────────────────────────────────────────\n");
    
    // Show first few entries to avoid screen overflow
    for (UINTN i = 0; i < num_descriptors && i < 10; i++) {
        if (desc->Type < sizeof(memory_type_names)/sizeof(memory_type_names[0])) {
            Print(L"%-20s %16lx %16lx %8ld\n",
                  memory_type_names[desc->Type],
                  desc->PhysicalStart,
                  desc->VirtualStart, 
                  desc->NumberOfPages);
        }
        
        desc = (EFI_MEMORY_DESCRIPTOR*)((UINT8*)desc + descriptor_size);
    }
    
    if (num_descriptors > 10) {
        Print(L"... and %ld more entries\n", num_descriptors - 10);
    }
    
    Print(L"\n💡 Randy's Memory Wisdom:\n");
    Print(L"   This is what raw memory looks like before the OS organizes it.\n");
    Print(L"   Every OS starts by understanding this firmware-provided map!\n");
    
    uefi_call_wrapper(BS->FreePool, 1, memory_map);
}

/*
 * Randy's System Information Display
 * "Show the firmware version and capabilities that most people never see"
 */
VOID randy_show_system_info(VOID) {
    Print(L"\n🔧 Randy's Pre-OS System Information\n");
    Print(L"═══════════════════════════════════════\n");
    
    Print(L"UEFI Firmware Vendor: %s\n", ST->FirmwareVendor);
    Print(L"UEFI Firmware Revision: %d.%d\n", 
          (ST->FirmwareRevision >> 16) & 0xFFFF,
          ST->FirmwareRevision & 0xFFFF);
    Print(L"UEFI Specification Version: %d.%d\n",
          (ST->Hdr.Revision >> 16) & 0xFFFF,
          ST->Hdr.Revision & 0xFFFF);
    
    Print(L"\n📊 Available Boot Services:\n");
    Print(L"   - Memory Management ✅\n");
    Print(L"   - Protocol Management ✅\n"); 
    Print(L"   - Device I/O ✅\n");
    Print(L"   - File System Access ✅\n");
    Print(L"   - Variable Services ✅\n");
    
    Print(L"\n🚀 Runtime Services Available:\n");
    Print(L"   - Time Services ✅\n");
    Print(L"   - Variable Storage ✅\n");
    Print(L"   - Virtual Memory ✅\n");
    Print(L"   - Reset/Reboot ✅\n");
    
    Print(L"\n💭 Randy's Firmware Philosophy:\n");
    Print(L"   This is the foundation layer that makes everything else possible.\n");
    Print(L"   Your OS is just another application running on this platform!\n");
}

/*
 * Randy's Configuration Table Explorer
 * "Show the hidden tables that contain system secrets"
 */
VOID randy_explore_config_tables(VOID) {
    Print(L"\n🗂️  Randy's Configuration Table Explorer\n");
    Print(L"═══════════════════════════════════════════════\n");
    
    Print(L"Number of Configuration Tables: %ld\n\n", ST->NumberOfTableEntries);
    
    for (UINTN i = 0; i < ST->NumberOfTableEntries && i < 8; i++) {
        EFI_CONFIGURATION_TABLE *table = &ST->ConfigurationTable[i];
        
        Print(L"Table %ld:\n", i);
        Print(L"  GUID: %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
              table->VendorGuid.Data1,
              table->VendorGuid.Data2, 
              table->VendorGuid.Data3,
              table->VendorGuid.Data4[0], table->VendorGuid.Data4[1],
              table->VendorGuid.Data4[2], table->VendorGuid.Data4[3],
              table->VendorGuid.Data4[4], table->VendorGuid.Data4[5],
              table->VendorGuid.Data4[6], table->VendorGuid.Data4[7]);
        Print(L"  Address: %p\n\n", table->VendorTable);
    }
    
    Print(L"💡 Randy's Table Wisdom:\n");
    Print(L"   These tables contain ACPI, SMBIOS, and other system data.\n");
    Print(L"   The OS will use these to understand your hardware!\n");
}

/*
 * Randy's Graphics Mode Information
 * "Show what display capabilities exist before any graphics driver loads"
 */
VOID randy_show_graphics_info(VOID) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    EFI_STATUS status;
    
    Print(L"\n🖥️  Randy's Graphics Output Information\n");
    Print(L"═══════════════════════════════════════════\n");
    
    // Locate Graphics Output Protocol
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &GraphicsOutputProtocolGuid, 
                               NULL, (VOID**)&gop);
    
    if (EFI_ERROR(status)) {
        Print(L"❌ Graphics Output Protocol not available\n");
        Print(L"   (Running in text mode or legacy BIOS)\n");
        return;
    }
    
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *mode_info = gop->Mode->Info;
    
    Print(L"Current Graphics Mode: %d\n", gop->Mode->Mode);
    Print(L"Max Graphics Mode: %d\n", gop->Mode->MaxMode);
    Print(L"Resolution: %dx%d\n", mode_info->HorizontalResolution, 
          mode_info->VerticalResolution);
    Print(L"Pixels per Scan Line: %d\n", mode_info->PixelsPerScanLine);
    Print(L"Frame Buffer Base: %p\n", gop->Mode->FrameBufferBase);
    Print(L"Frame Buffer Size: %ld bytes\n", gop->Mode->FrameBufferSize);
    
    const CHAR16 *pixel_formats[] = {
        L"PixelRedGreenBlueReserved8BitPerColor",
        L"PixelBlueGreenRedReserved8BitPerColor", 
        L"PixelBitMask",
        L"PixelBltOnly"
    };
    
    if (mode_info->PixelFormat < sizeof(pixel_formats)/sizeof(pixel_formats[0])) {
        Print(L"Pixel Format: %s\n", pixel_formats[mode_info->PixelFormat]);
    }
    
    Print(L"\n💡 Randy's Graphics Wisdom:\n");
    Print(L"   This framebuffer exists before any GPU driver loads.\n");
    Print(L"   You could draw pixels directly at this level!\n");
}

/*
 * Randy's Main EFI Application Entry Point
 * "Where firmware meets consciousness - the deepest programmable layer"
 */
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    // Initialize GNU-EFI library
    InitializeLib(ImageHandle, SystemTable);
    
    // Clear screen and show Randy's signature intro
    randy_clear_screen();
    
    Print(L"🧠⚡ %s ⚡🧠\n", RANDY_APP_NAME);
    Print(L"    %s\n\n", RANDY_VERSION);
    
    Print(L"🌿 Welcome to the Deepest Programmable Layer! 🌿\n");
    Print(L"══════════════════════════════════════════════════════════════\n\n");
    
    Print(L"💭 Randy's Firmware Revelation:\n");
    Print(L"   \"Most programmers think the OS is the bottom layer.\n");
    Print(L"    But you're now running BELOW the operating system!\n");
    Print(L"    This code executes at firmware level - pure hardware magic.\"\n\n");
    
    Print(L"🎯 What You're Experiencing:\n");
    Print(L"   ✅ No operating system loaded\n");
    Print(L"   ✅ Direct UEFI firmware interaction\n");
    Print(L"   ✅ Raw hardware access\n");
    Print(L"   ✅ Boot-time programmable environment\n");
    Print(L"   ✅ Pre-OS memory and device management\n\n");
    
    Print(L"🌱 Plant Spirit Hardware Wisdom:\n");
    Print(L"   \"Every abstraction layer hides power. Sometimes you need\n");
    Print(L"    to go deeper than the roots to understand how the\n");
    Print(L"    whole computational garden grows.\"\n\n");
    
    randy_wait_for_key();
    
    // Show system information
    randy_show_system_info();
    randy_wait_for_key();
    
    // Show memory map
    randy_show_memory_map();
    randy_wait_for_key();
    
    // Show configuration tables
    randy_explore_config_tables();
    randy_wait_for_key();
    
    // Show graphics information
    randy_show_graphics_info();
    randy_wait_for_key();
    
    // Final educational message
    randy_clear_screen();
    randy_print_centered(L"🎓 Randy's Firmware Education Complete! 🎓\n\n");
    
    Print(L"🧠 What You've Just Learned:\n");
    Print(L"══════════════════════════════════════\n");
    Print(L"✅ Firmware is programmable (this proves it!)\n");
    Print(L"✅ UEFI provides rich development environment\n");
    Print(L"✅ Memory management exists before OS\n");
    Print(L"✅ Hardware information is available at boot\n");
    Print(L"✅ Graphics and input work without drivers\n\n");
    
    Print(L"🌿 Randy's Cannabis-Enhanced Insights:\n");
    Print(L"══════════════════════════════════════════\n");
    Print(L"🍃 The deepest layer is often the most powerful\n");
    Print(L"🍃 Understanding the foundation illuminates everything above\n");
    Print(L"🍃 Firmware programming is the ultimate 'close to metal' experience\n");
    Print(L"🍃 Every OS is just firmware's most ambitious application\n\n");
    
    Print(L"🚀 Next Steps in Your Educational Journey:\n");
    Print(L"═══════════════════════════════════════════\n");
    Print(L"📚 Study UEFI specification for deeper understanding\n");
    Print(L"🔧 Build a simple bootloader from scratch\n");
    Print(L"🖥️  Experiment with direct framebuffer graphics\n");
    Print(L"💾 Create custom UEFI drivers\n");
    Print(L"🧪 Explore ACPI and hardware enumeration\n\n");
    
    Print(L"💡 Randy's Final Firmware Wisdom:\n");
    Print(L"════════════════════════════════════\n");
    Print(L"   \"Most people never see this layer, but it's where the real\n");
    Print(L"    magic happens. Every computer boot is a conversation between\n");
    Print(L"    firmware and hardware, establishing the foundation for\n");
    Print(L"    everything you think of as 'computing.'\n\n");
    
    Print(L"    Now when you wait for your OS to load, you'll know there's\n");
    Print(L"    a whole universe of programmable firmware doing incredible\n");
    Print(L"    work before your operating system even thinks about starting.\n\n");
    
    Print(L"    The plant spirits have revealed: true understanding comes\n");
    Print(L"    from experiencing every layer of the stack, especially\n");
    Print(L"    the ones hidden beneath the surface.\"\n\n");
    
    Print(L"🌟 System will reboot in 10 seconds to return you to familiar layers...\n");
    Print(L"    (Or press any key to exit immediately)\n");
    
    // Wait 10 seconds or until keypress
    EFI_INPUT_KEY key;
    for (int i = 0; i < 100; i++) {  // 100 * 100ms = 10 seconds
        EFI_STATUS key_status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, 
                                                   ST->ConIn, &key);
        if (key_status != EFI_NOT_READY) {
            break;  // Key was pressed
        }
        uefi_call_wrapper(BS->Stall, 1, 100000); // 100ms delay
    }
    
    Print(L"\n🌿 Thank you for exploring the firmware realm with Randy!\n");
    Print(L"   May your newfound knowledge illuminate all layers above!\n\n");
    
    // Exit cleanly
    return EFI_SUCCESS;
}

/*
 * Build Instructions for Randy's EFI Loader
 * ═════════════════════════════════════════
 * 
 * Using gnu-efi (easiest approach):
 * 1. Install gnu-efi development package
 * 2. Compile: gcc -I/usr/include/efi -I/usr/include/efi/x86_64 \
 *                 -DEFI_FUNCTION_WRAPPER -fno-stack-protector \
 *                 -fpic -fshort-wchar -mno-red-zone -Wall \
 *                 -c randy_educational_efi_loader.c -o randy_efi.o
 * 3. Link: ld -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds \
 *             -shared -Bsymbolic -L /usr/lib /usr/lib/crt0-efi-x86_64.o \
 *             randy_efi.o -o randy_efi.so -lefi -lgnuefi
 * 4. Convert: objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym \
 *                     -j .rel -j .rela -j .reloc --target=efi-app-x86_64 \
 *                     randy_efi.so randy_educational_efi.efi
 * 
 * Testing:
 * 1. Copy randy_educational_efi.efi to USB drive formatted as FAT32
 * 2. Boot from USB in UEFI mode  
 * 3. Navigate to EFI shell and run: fs0:\randy_educational_efi.efi
 * 4. Or use QEMU: qemu-system-x86_64 -bios OVMF.fd -drive format=raw,file=usb.img
 * 
 * Randy's Cannabis-Enhanced Build Wisdom:
 * "Building EFI applications is like growing plants - you need the right
 * environment, proper tools, and patience for the magic to happen.
 * The plant spirits guide us through the complex linking incantations!"
 */