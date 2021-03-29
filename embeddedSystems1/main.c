#include <stdio.h>
#include <Windows.h>

int main(int argc, const char* argv[]) {
    if(argc != 2) {
        printf("Usage: embeddedSystems1 pe_file.exe");
        return 0;
    }
    FILE *file_PE;
    if ((file_PE = fopen(argv[1], "rb")) == NULL) {
        printf("Can't open file");
        return 0;
    }
    FILE *file_out_info;
    if ((file_out_info = fopen("../info.txt", "w")) == NULL) {
        printf("Can't open file");
        return 0;
    }
    if (file_PE == NULL)
        return 0;
    IMAGE_DOS_HEADER dos_header;
    fread((&dos_header), sizeof(IMAGE_DOS_HEADER), 1, file_PE);
    if (dos_header.e_magic != IMAGE_DOS_SIGNATURE){
        fprintf(file_out_info, "IMAGE_DOS_HEADER signature is incorrect\n");
        return 0;
    }
    if((dos_header.e_lfanew % sizeof(DWORD)) != 0) {
        fprintf(file_out_info, "PE header is not DWORD-aligned\n");
        return 0;
    }
    fseek(file_PE, dos_header.e_lfanew, SEEK_SET);
    IMAGE_NT_HEADERS pHeader;
    fread((&pHeader), sizeof(IMAGE_NT_HEADERS), 1, file_PE);
    if (pHeader.Signature != IMAGE_NT_SIGNATURE) {
        fprintf(file_out_info, "Wrong IMAGE_NT_SIGNATURE\n");
        return 0;
    }
    fprintf(file_out_info, "Address of Entry Point: %lu\n\n", pHeader.OptionalHeader.AddressOfEntryPoint);
    DWORD first_section = dos_header.e_lfanew + pHeader.FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + sizeof(DWORD);
    fseek(file_PE, first_section, SEEK_SET);
    DWORD pCode;
    DWORD pSize;
    for(int i = 0; i < pHeader.FileHeader.NumberOfSections; i++) {
        IMAGE_SECTION_HEADER pSectHeader;
        fread((&pSectHeader), sizeof(IMAGE_SECTION_HEADER), 1, file_PE);
        char name[9] = {0};
        memcpy(name, pSectHeader.Name, 8);
        fprintf(file_out_info, "Section: %s \n" , name);
        fprintf(file_out_info, "Virtual size: %lu\n", pSectHeader.Misc.VirtualSize);
        fprintf(file_out_info, "Raw size: %lu\n", pSectHeader.SizeOfRawData);
        fprintf(file_out_info, "Virtual address: %lu\n", pSectHeader.VirtualAddress);
        fprintf(file_out_info, "Raw address: %lu\n", pSectHeader.PointerToRawData);

        if (strcmp(name, ".text") != 0) {
            pCode = pSectHeader.PointerToRawData;
            pSize = pSectHeader.SizeOfRawData;
        }

        fprintf(file_out_info, "Characteristics: ");
        if(pSectHeader.Characteristics & IMAGE_SCN_MEM_READ)
            fprintf(file_out_info, "R ");
        if(pSectHeader.Characteristics & IMAGE_SCN_MEM_WRITE)
            fprintf(file_out_info, "W ");
        if(pSectHeader.Characteristics & IMAGE_SCN_MEM_EXECUTE)
            fprintf(file_out_info, "X ");
        if (pSectHeader.Characteristics & IMAGE_SCN_CNT_CODE)
            fprintf(file_out_info, "C ");
        if(pSectHeader.Characteristics & IMAGE_SCN_MEM_DISCARDABLE)
            fprintf(file_out_info, "discardable ");
        if(pSectHeader.Characteristics & IMAGE_SCN_MEM_SHARED)
            fprintf(file_out_info, "shared");

        fprintf(file_out_info, "\n\n");
    }
    fseek(file_PE, pCode, SEEK_SET);
    char code[pSize];
    FILE *file_out_code;
    if ((file_out_code = fopen("../binary_code.bin", "wb")) == NULL) {
        printf("Can't open file");
        return 0;
    }
    fread((&code), sizeof(byte), pSize, file_PE);
    fwrite((&code), sizeof(byte), pSize, file_out_code);
    fclose(file_out_code);
    fclose(file_out_info);
    fclose(file_PE);
    fseek(file_PE, 0L, SEEK_SET);
    return 0;
}

