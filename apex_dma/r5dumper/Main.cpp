#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

std::string FILE_NAME;
std::vector<std::string>* lines = new std::vector<std::string>;
std::map<std::string, uint32_t> mapConVars;
std::map<std::string, uint32_t> mapOffsets;
std::map<std::string, uint32_t> mapButtons;
char* memoryBytes;
size_t memoryBytesSize;

void trimLeft(std::string& s) {
    s.erase(s.begin(),
        std::find_if(s.begin(), s.end(), [](unsigned char ch){ return !std::isspace(ch); }));
}

void trimRight(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch){ return !std::isspace(ch); }).base(),
            s.end());
}

void trimStr(std::string& s) {
    trimLeft(s);
    trimRight(s);
}

std::string formStr(std::string s) {
    std::replace(s.begin(), s.end(), ' ', '_');
    return s;
}

bool loadFileIntoVector() {
    lines->clear();
    std::string str;
    std::ifstream myFile(FILE_NAME);
    while (getline(myFile, str)) {
        trimStr(str);
        if (str.empty()) continue;
        if (str.rfind("#", 0) == 0) continue;
        lines->push_back(str);
    }
    myFile.close();
    return true;
}

std::vector<std::string> split(std::string s) {
    std::stringstream ss(s);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> tokens(begin, end);
    return tokens;
}

std::vector<uint8_t> getPatternBytes(std::string pattern)
{
    std::vector<uint8_t> bytes;
    for (int i = 0; i < pattern.size();) {
        if (pattern[i] == ' ') { i += 1; continue; }
        if (pattern[i] == '?') {
            bytes.push_back(0);
            i += 1;  // ?
            continue;
        }
        if (pattern[i] == '%') {
            i += 1;  // %
            continue;
        }
        if (pattern[i] == '$' && pattern[i+1] == '{') {
            i += 2;  // ${
            continue;
        }
        if (pattern[i] == '\'') {
            i += 1;  // '
            continue;
        }
        if (pattern[i] == '}') {
            i += 1;  // }
            continue;
        }
        if (pattern[i] == 'u' && pattern[i+1] == '4') {
            i += 2;  // u4
            continue;
        }
        bytes.push_back(std::stoi(pattern.substr(i, 2), nullptr, 16));
        i += 2;
    }
    return bytes;
}

bool scanForPattern(size_t &resume, size_t size, std::string pattern, uint32_t (&save)[4], uint32_t (&saveAddr)[4])
{
    std::vector<uint8_t> patternBytes;
    patternBytes = getPatternBytes(pattern);
    int32_t jump;
    size_t cursorMemoryPrev;
    for (size_t i = resume; i < size - patternBytes.size(); i++) {
        bool found = true;
        size_t cursorMemory = 0;
        size_t cursorPattern = 0;
        uint8_t saveSlot = 0;
        for (int j = 0; j < pattern.size();) {
            if (pattern[j] <= '9' && pattern[j] >= '0' || pattern[j] <= 'F' && pattern[j] >= 'A') {
                if (i+cursorMemory < size)
                    found = patternBytes[cursorPattern] == (uint8_t)memoryBytes[i+cursorMemory];
                else
                    found = false;
                cursorMemory += 1;
                cursorPattern += 1;
                j += 2;
                if (!found)
                    break;
            }else
            if (pattern[j] == ' ') {
                j += 1;
            }else
            if (pattern[j] == '?') {
                cursorMemory += 1;
                cursorPattern += 1;
                j += 1;  // ?
            }else
            if (pattern[j] == 'u' && pattern[j+1] == '4') {
                save[saveSlot] = *(uint32_t*)(memoryBytes + i + cursorMemory);
                saveAddr[saveSlot] = i + cursorMemory;
                saveSlot++;
                cursorMemory += 4;
                j += 2;  // u4
            }else
            if (pattern[j] == '$' && pattern[j+1] == '{') {
                jump = *(int32_t*)(memoryBytes + i + cursorMemory);
                cursorMemoryPrev = cursorMemory;
                cursorMemory += 4;  // Skip 4 bytes used for jump
                cursorMemory += jump;
//                printf("jump: 0x%.08X ** save: 0x%.08X ** data: %.08X\n", jump, save, *(uint32_t*)(memoryBytes + i + cursorMemory));
                j += 2;  // ${
            }else
            if (pattern[j] == '\'') {
                save[saveSlot] = i + cursorMemory;
                saveAddr[saveSlot] = i + cursorMemoryPrev;
                saveSlot++;
                j += 1;  // '
            }else
            if (pattern[j] == '}') {
                cursorMemory = cursorMemoryPrev + 4;
                j += 1;  // }
            }else
            if (pattern[j] == '%') {
                jump = *(int8_t*)(memoryBytes + i + cursorMemory);
                cursorMemory += 1;  // Skip 1 byte used for jump
                cursorMemory += jump;
//                printf("jump: %d ** data: %.08X\n", jump, *(uint32_t*)(memoryBytes + i + cursorMemory));
                j += 1;  // %
            }else { found = false; break; }
        }
        if (found) {
            resume = i;
            return true;
        }
    }
    return false;
}


int main(int argc, char* argv[])
{
    FILE_NAME = "_convars.ini";
    if (loadFileIntoVector()) {
        for (int i = 0; i < lines->size(); i++) {
            std::vector<std::string> lineParts = split(lines->at(i));
            // Line key
            std::string key(lineParts.at(0));
            trimStr(key);
            // Line value
            std::string value(lineParts.at(1));
            trimStr(value);

            mapConVars[key] = std::stoul(value, nullptr, 16);
        }
    }
//    for (auto i = mapConVars.cbegin(); i != mapConVars.cend(); ++i)
//        std::cout << i->first << " 0x" << i->second << "\n";


    FILE_NAME = "_offsets.ini";
    if (loadFileIntoVector()) {
        for (int i = 0; i < lines->size(); i++) {
            std::vector<std::string> lineParts = split(lines->at(i));
            // Line key
            std::string key(lineParts.at(0));
            trimStr(key);
            // Line value
            std::string value(lineParts.at(1));
            trimStr(value);

            mapOffsets[key] = std::stoul(value, nullptr, 16);
    }
}
//    for (auto i = mapOffsets.cbegin(); i != mapOffsets.cend(); ++i)
//        std::cout << i->first << " 0x" << i->second << "\n";


    FILE_NAME = "_buttons.ini";
    if (loadFileIntoVector()) {
        for (int i = 0; i < lines->size(); i++) {
            std::vector<std::string> lineParts = split(lines->at(i));
            // Line key
            std::string key(lineParts.at(0));
            trimStr(key);
            // Line value
            std::string value(lineParts.at(1));
            trimStr(value);

            mapButtons[key] = std::stoul(value, nullptr, 16);
        }
    }
//    for (auto i = mapButtons.cbegin(); i != mapButtons.cend(); ++i)
//        std::cout << i->first << " 0x" << i->second << "\n";


    std::string targetFile;
    if (argv[1]) {
        targetFile = std::string(argv[1]);
    } else {
        std::cout << "usage: ./r5dumper dumpfile.bin [\"? 48 8B 05 \\${???????? '} 488D0D???? 4488????? 4C89\"]\n";
        return 0;
    }
    memoryBytesSize = std::filesystem::file_size(targetFile);
    memoryBytes = (char*)std::malloc(memoryBytesSize);
    std::cout << targetFile << " " << memoryBytesSize/1024/1024 << " MiB\n";
    std::ifstream fs(targetFile, std::ios::in | std::ios::binary);
    fs.read(memoryBytes, memoryBytesSize);

    size_t resume, maxStep;
    uint32_t save[4], saveAddr[4];

    if (argv[2]) {
        printf("%s\n", argv[2]);
        maxStep = 0x2000000;
        for (size_t step = 0; step < maxStep;) {
            resume = step;
            saveAddr[0] = 0;
            if (scanForPattern(resume, maxStep, argv[2], save, saveAddr)) {
                printf("%08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X @ %x\n",
                    *(uint32_t*)(-16+ memoryBytes + resume), *(uint32_t*)(-12+ memoryBytes + resume),
                    *(uint32_t*)(-8 + memoryBytes + resume), *(uint32_t*)(-4 + memoryBytes + resume),
                    *(uint32_t*)(   + memoryBytes + resume), *(uint32_t*)( 4 + memoryBytes + resume),
                    *(uint32_t*)( 8 + memoryBytes + resume), *(uint32_t*)(12 + memoryBytes + resume),
                    *(uint32_t*)(16 + memoryBytes + resume), *(uint32_t*)(20 + memoryBytes + resume),
                    *(uint32_t*)(24 + memoryBytes + resume), *(uint32_t*)(28 + memoryBytes + resume),
                    resume);
                if (saveAddr[0] > 0) for (int j = 0; j < 4 + (saveAddr[0] - resume) / 4; j++) fprintf(stderr, "         ");
                if (saveAddr[0] < memoryBytesSize)
                    printf("%08X=%08X\n", *(uint32_t*)(memoryBytes + saveAddr[0]), *(uint32_t*)(memoryBytes + saveAddr[0]) + saveAddr[0] + 4);
                step = resume + 4;
            } else step = maxStep;
        }
        return 0;
    }


  /*struct RawConVar
    {
        uint64_t m_vTable;           // 0x0000
        uint64_t m_pNext;            // 0x0008
      //uint64_t m_bRegistered;      // 0x0010
        uint64_t m_pszName;          // 0x0018  // 0x0010 since 2025-Jun-24
        uint64_t m_pszDescription;   // 0x0020
        uint64_t m_pszDataType;      // 0x0028
        uint32_t m_iFlags;           // 0x0030
        char     pad_0034[4];        // 0x0034
        uint64_t m_pParent;          // 0x0038
        uint64_t m_pTable;           // 0x0040
        uint64_t m_pszDefaultValue;  // 0x0048
        uint64_t m_pszString;        // 0x0050
        uint64_t m_iStringLength;    // 0x0058
        float    m_fValue;           // 0x0060
        int32_t  m_nValue;           // 0x0064
        bool     m_bHasMin;          // 0x0068
        bool     m_bHasMax;          // 0x0069
        char     pad_006A[2];        // 0x006A
        float    m_fMinValue;        // 0x006C
        float    m_fMaxValue;        // 0x0070
    };*/
    uint32_t e_lfanew       = *(uint32_t*)(memoryBytes + 0x3C);
    uint64_t ImageBase      = *(uint64_t*)(memoryBytes + e_lfanew + 0x30);
    uint64_t g_base = ImageBase;
    uint32_t SizeOfImage    = *(uint32_t*)(memoryBytes + e_lfanew + 0x50);
    char dataName[8] = {0};
    std::memcpy(dataName,                       &memoryBytes[e_lfanew + 0x158], 7);  // .data
    uint32_t dataVirtualSize    = *(uint32_t*)(memoryBytes + e_lfanew + 0x160);      // .data
    uint32_t dataVirtualAddress = *(uint32_t*)(memoryBytes + e_lfanew + 0x164);      // .data
    printf("ImageBase=0x%lx SizeOfImage=0x%x\n", ImageBase, SizeOfImage);
    printf("dataName=%s dataVirtualSize=0x%x dataVirtualAddress=0x%x\n", dataName, dataVirtualSize, dataVirtualAddress);
    char textName[8] = {0};
    std::memcpy(textName,                       &memoryBytes[e_lfanew + 0x108], 7);  // .text
    uint32_t textVirtualSize    = *(uint32_t*)(memoryBytes + e_lfanew + 0x110);      // .text
    uint32_t textVirtualAddress = *(uint32_t*)(memoryBytes + e_lfanew + 0x114);      // .text
    printf("textName=%s textVirtualSize=0x%x textVirtualAddress=0x%x\n", textName, textVirtualSize, textVirtualAddress);
    uint64_t vftable;
    resume = 0;
    if (scanForPattern(resume, memoryBytesSize, "48 8B 79 ? 48 8D 05 ${'} 48 89 ? 48 8D", save, saveAddr))  //488B79? 488D05${'} 4889? 488D
        vftable = g_base + save[0];
    maxStep = dataVirtualSize / 8;

    int offset;
    uint32_t gameModeFound = 0;
    for (size_t step = 0; step < maxStep; step++) {
        char nameConVar[128] = {0};
        uint32_t addressConVar = dataVirtualAddress + step * 8;
        uint64_t ptr = *(uint64_t*)(memoryBytes + addressConVar);
        if (ptr == vftable) {
            for (int i = 0x0008; i < 0x0034; i++) {
                uint64_t m_pszName = *(uint64_t*)(memoryBytes + addressConVar + i);
                m_pszName -= g_base;
                if (m_pszName < memoryBytesSize) {
                    std::memcpy(nameConVar, &memoryBytes[m_pszName], 127);
                    if (strncmp(nameConVar, "mp_gamemode", 11) == 0) {
                        std::cout << nameConVar << "=0x" << std::hex << addressConVar << "\n";
                        offset = i;
                        gameModeFound = addressConVar;
                        break;
                    }
                }
            }
            if (gameModeFound > 0) break;
        }
    }

    int conVarsCount = 0;
    if (gameModeFound > 0) {
        for (size_t step = 0; step < maxStep; step++) {
            char nameConVar[128] = {0};
            uint32_t addressConVar = dataVirtualAddress + step * 8;
            uint64_t ptr = *(uint64_t*)(memoryBytes + addressConVar);
            if (ptr == vftable) {
                uint64_t m_pszName = *(uint64_t*)(memoryBytes + addressConVar + offset);
                m_pszName -= g_base;
                if (m_pszName < memoryBytesSize) {
                    std::memcpy(nameConVar, &memoryBytes[m_pszName], 127);
                    std::string fullName = formStr("[ConVars]" + std::string(nameConVar));
                    if (mapConVars[fullName] == 0)
                        mapConVars[fullName] = addressConVar;
                }
            }
        }
        std::ofstream fileConVars("_convars.ini");
        for (auto i = mapConVars.cbegin(); i != mapConVars.cend(); ++i) {
            fileConVars << i->first << " " << std::hex << i->second << "\n";
            conVarsCount++;
        }
        fileConVars.close();
    }
    std::cout << "\n" << std::dec << conVarsCount << " Convars\n";


    int dataMapsCount = 0;
    int dataFieldsCount = 0;
  /*struct DataMap
    {
        uint64_t m_dataDesc;       // 0x0000
        uint32_t m_dataNumFields;  // 0x0008
        uint32_t _pad0;            // 0x000C
        uint64_t m_dataClassName;  // 0x0010
        uint64_t _pad1;            // 0x0018
        uint64_t _pad2;            // 0x0020
        uint64_t m_baseMap;        // 0x0028
    };*/
    struct DataTypeDesc
    {
        int32_t  m_fieldType;
        uint32_t _pad0;
        uint64_t m_fieldName;
        uint32_t m_fieldOffset[2];
        uint64_t m_externalName;
        uint64_t _pad1[5];
        uint64_t m_td;
        int32_t  m_fieldSizeInBytes;
        uint64_t _pad2[5];
    };
    maxStep = textVirtualAddress + textVirtualSize;
    for (size_t step = textVirtualAddress; step < maxStep;) {
        resume = step;
        if (scanForPattern(resume, maxStep, "48 8D 05 ${'} C3", save, saveAddr)) {
            uint64_t m_dataDesc      = *(uint64_t*)(memoryBytes + save[0]         );
            uint32_t m_dataNumFields = *(uint32_t*)(memoryBytes + save[0] + 0x0008);
            uint64_t m_dataClassName = *(uint64_t*)(memoryBytes + save[0] + 0x0010);
            m_dataClassName -= g_base;
            if (m_dataClassName < memoryBytesSize && m_dataNumFields >= 0x0001 && m_dataNumFields <= 0x1000) {
                auto dataClassName = (char*)(memoryBytes + m_dataClassName);
                m_dataDesc -= g_base;
                auto dataTypeDesc = reinterpret_cast<const DataTypeDesc*>(memoryBytes + m_dataDesc);
                for (int i = 0; i < m_dataNumFields; i++) {
                    uint64_t m_fieldName = dataTypeDesc[i].m_fieldName;
                    m_fieldName -= g_base;
                    if (m_fieldName < memoryBytesSize) {
                        if (i == 0)
                            std::cout << "\n[DataMap." << dataClassName << "]\n";
                        auto fieldName = (char*)(memoryBytes + m_fieldName);
                        uint32_t m_fieldOffset = dataTypeDesc[i].m_fieldOffset[0];
                        std::cout << fieldName << " = 0x" << std::hex << m_fieldOffset << "\n";
                        std::string fullName = formStr("[DataMap." + std::string(dataClassName) + "]" + fieldName);
                        if (mapOffsets[fullName] == 0)
                            mapOffsets[fullName] = m_fieldOffset;
                        dataFieldsCount++;
                    }
                }
                dataMapsCount++;
            }
            step = resume + 8;
        } else { break; }
    }
    std::cout << "\n" << std::dec << dataMapsCount << " DataMaps, " << dataFieldsCount << " DataFields\n";


#define PRINT_TABLE(recvTableSave, recvPropSave, recvTablesCount, recvPropsCount) ({\
    auto recvTable = reinterpret_cast<const RecvTable*>(memoryBytes + recvTableSave);\
    auto recvProp  = reinterpret_cast<const  RecvProp*>(memoryBytes + recvPropSave);\
    uint64_t m_tableName = recvTable->m_name;\
    m_tableName -= g_base;\
    if (m_tableName < memoryBytesSize && recvTable->m_iProps > 0 && recvTable->m_iProps <= 0x1000) {\
        auto tableName = (char*)(memoryBytes + m_tableName);\
        std::cout << "\n[RecvTable." << tableName << "]\n";\
        for (int i = 0; i < recvTable->m_iProps; i++) {\
            uint64_t m_propName = recvProp[i].m_name;\
            m_propName -= g_base;\
            if (m_propName < memoryBytesSize && recvProp[i].m_offset > 0) {\
                auto propName = (char*)(memoryBytes + m_propName);\
                std::cout << propName << " = 0x" << std::hex << recvProp[i].m_offset << "\n";\
                std::string fullName = formStr("[RecvTable." + std::string(tableName) + "]" + propName);\
                if (mapOffsets[fullName] == 0)\
                    mapOffsets[fullName] = recvProp[i].m_offset;\
                recvPropsCount++;\
            }\
        }\
        recvTablesCount++;\
    }\
})


    struct RecvTable
    {
        uint64_t _pad0;
        uint64_t m_pProps;
        int32_t  m_iProps;
        uint32_t _pad1[256];
        uint32_t _pad2[43];
        uint64_t m_decoder;
        uint64_t m_name;
        bool     m_initialized;
        bool     m_inMainList;
        uint8_t  _pad3[6];
    };
    struct RecvProp
    {
        int32_t  m_type;
        int32_t  m_offset;
        uint32_t _pad0[6];
        uint64_t m_dataTable;
        uint64_t m_name;
        bool     m_isInsideArray;
        uint8_t  _pad1[7];
        uint64_t m_arrayProp;
        uint64_t m_proxyFn;
        uint32_t _pad2[4];
        int32_t  m_flags;
        uint32_t _pad3;
        int32_t  m_iNumElements;
        uint32_t _pad4;
    };
    maxStep = textVirtualAddress + textVirtualSize;
    int recvTablesCount13 = 0;
    int recvPropsCount13 = 0;
    for (size_t step = textVirtualAddress; step < maxStep;) {
        resume = step;
        if (scanForPattern(resume, maxStep, "488D0D${'} C705${'} ???? 48891D", save, saveAddr)) {
            printf("%08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X @ %x\n",
                *(uint32_t*)(-16+ memoryBytes + resume), *(uint32_t*)(-12+ memoryBytes + resume),
                *(uint32_t*)(-8 + memoryBytes + resume), *(uint32_t*)(-4 + memoryBytes + resume),
                *(uint32_t*)(   + memoryBytes + resume), *(uint32_t*)( 4 + memoryBytes + resume),
                *(uint32_t*)( 8 + memoryBytes + resume), *(uint32_t*)(12 + memoryBytes + resume),
                *(uint32_t*)(16 + memoryBytes + resume), *(uint32_t*)(20 + memoryBytes + resume),
                *(uint32_t*)(24 + memoryBytes + resume), *(uint32_t*)(28 + memoryBytes + resume),
                resume);
            PRINT_TABLE(save[1] + 4 - 16, save[0], recvTablesCount13, recvPropsCount13);
            step = resume + 20;
        } else { break; }
    }
    std::cout << "\n[1/3] " << std::dec << recvTablesCount13 << " RecvTables, " << recvPropsCount13 << " RecvProps\n";

    int recvTablesCount23 = 0;
    int recvPropsCount23 = 0;
    for (size_t step = textVirtualAddress; step < maxStep;) {
        resume = step;
        if (scanForPattern(resume, maxStep, "4C8D0D${'} 41B8u4 488D15${'} 488D0D${'} E8", save, saveAddr)) {
            PRINT_TABLE(save[3], save[2], recvTablesCount23, recvPropsCount23);
            step = resume + 28;
        } else { break; }
    }
    std::cout << "\n[2/3] " << std::dec << recvTablesCount23 << " RecvTables, " << recvPropsCount23 << " RecvProps\n";

    int recvTablesCount33 = 0;
    int recvPropsCount33 = 0;
    for (size_t step = textVirtualAddress; step < maxStep;) {
        resume = step;
        if (scanForPattern(resume, maxStep, "488D15${'} 41B8u4 488D0D${'} E8", save, saveAddr)) {
            PRINT_TABLE(save[2], save[0], recvTablesCount33, recvPropsCount33);
            step = resume + 21;
        } else { break; }
    }
    std::cout << "\n[3/3] " << std::dec << recvTablesCount33 << " RecvTables, " << recvPropsCount33 << " RecvProps\n";


//488D0D${\"Attempted to perform GetMods\"} [40-80] 399Eu4 [2-10] 8D?u4 [20-50] 488B15${*'}
//488D0D ${\"Attempted to perform GetMods\"}
//488D0D ${417474656D7074656420746F 20706572666F726D 204765744D6F6473}

    std::cout << "\n[ModifierOffsets]\n";
    int modifierNamesCount = 0;
    resume = 0;
    if (scanForPattern(resume, dataVirtualAddress, "488D0D ${417474656D7074656420746F 20706572666F726D 204765744D6F6473}", save, saveAddr)) {
        uint32_t mods_count;
        uint64_t mods_names;
        size_t temp = resume + 35 + 20;
        if (scanForPattern(temp, temp + 40, "39 ? u4", save, saveAddr)) {
            mods_count = save[0];
            std::cout << "mods_count = 0x" << std::hex << mods_count << "\n";
            temp += 6 + 20;
            if (scanForPattern(temp, temp + 30, "48 8B 15 ${'}", save, saveAddr)) {
                mods_names = *(uint64_t*)(memoryBytes + save[0]);
                mods_names -= g_base;
                std::cout << "mods_names = 0x" << std::hex << mods_names << "\n";
                std::cout << "\n[ModifierNames]\n";
                for (int i = 0; i < mods_count; i++) {
                    uint64_t namePtr = mods_names + i*8;
                    if (namePtr >= memoryBytesSize - 8) break;
                    uint64_t nameAddr = *(uint64_t*)(memoryBytes + namePtr);
                    nameAddr -= g_base;
                    auto name = (char*)(memoryBytes + nameAddr);
                    if (strlen(name) == 0) break;
                    std::cout << name << " = " << std::dec << i << "\n";
                    std::string fullName = formStr("[ModifierNames]" + std::string(name));
                    if (mapOffsets[fullName] == 0)
                        mapOffsets[fullName] = i;
                    modifierNamesCount++;
                }
            }
        }
    }
    std::cout << "\n" << std::dec << modifierNamesCount << " ModifierNames\n";


    std::cout << "\n[NetworkedStringTables]\n";
    int networkedStringTablesCount = 0;
    maxStep = dataVirtualAddress;
    for (size_t step = 0; step < maxStep;) {
        resume = step;
        if (scanForPattern(resume, dataVirtualAddress, "488D15 ${'} 498BCBE8 ${4C8BC9}", save, saveAddr)) {
            auto nameNetworkedStringTable = (char*)(memoryBytes + save[0]);
            size_t temp = resume + 15;
            if (scanForPattern(temp, temp + 64, "488905 ${'}", save, saveAddr)) {
                std::cout << nameNetworkedStringTable << " = 0x" << std::hex << save[0] << "\n";
                std::string fullName = formStr("[NetworkedStringTables]" + std::string(nameNetworkedStringTable));
                if (mapOffsets[fullName] == 0)
                    mapOffsets[fullName] = save[0];
                networkedStringTablesCount++;
            }
            step = resume + 15;
        } else { break; }
    }
    std::cout << "\n" << std::dec << networkedStringTablesCount << " NetworkedStringTables\n";


    int weaponSettingsCount = 0;
    struct RawWeaponDataField
    {
        uint64_t name;
        uint64_t _default;
        uint64_t description;
//        uint32_t unk1;
//        uint32_t offset;
        uint8_t  ty;
        uint8_t  flags;
        uint16_t index;
        uint16_t offset;
        uint8_t  _pad[2];
    };
    resume = 0;
    if (scanForPattern(resume, dataVirtualAddress, "41 8D 40 FF 3D u4 0F 87", save, saveAddr)) {
        uint32_t weaponSettingsSize = save[0];
        resume = 0;
        if (scanForPattern(resume, dataVirtualAddress, "48 8D 05 ${'} 49 63 D0", save, saveAddr)) {
            uint32_t weaponSettings = save[0];
            std::cout << "\n[WeaponSettings]\n";\
            for (uint32_t i = 0; i < weaponSettingsSize * 2; ++i) {
                auto weaponDataField = reinterpret_cast<const RawWeaponDataField*>(memoryBytes + weaponSettings + i*sizeof(RawWeaponDataField));
                uint64_t weaponDataFieldName = weaponDataField->name;
                weaponDataFieldName -= g_base;
                if (weaponDataFieldName < memoryBytesSize) {
                    auto weaponName = (char*)(memoryBytes + weaponDataFieldName);
                    std::cout << weaponName << " = 0x" << weaponDataField->offset << "\n";
                    std::string fullName = formStr("[WeaponSettings]" + std::string(weaponName));
                    if (mapOffsets[fullName] == 0)
                        mapOffsets[fullName] = weaponDataField->offset;
                    weaponSettingsCount++;
                }
            }
        }
    }
    std::cout << "\n" << std::dec << weaponSettingsCount << " WeaponSettings\n";


    std::cout << "\n" << std::dec << conVarsCount << " Convars\n";
    std::cout << std::dec << dataMapsCount << " DataMaps, " << dataFieldsCount << " DataFields" << "\n";
    std::cout << "[1/3] " << std::dec << recvTablesCount13 << " RecvTables, " << recvPropsCount13 << " RecvProps\n";
    std::cout << "[2/3] " << std::dec << recvTablesCount23 << " RecvTables, " << recvPropsCount23 << " RecvProps\n";
    std::cout << "[3/3] " << std::dec << recvTablesCount33 << " RecvTables, " << recvPropsCount33 << " RecvProps\n";
    std::cout << std::dec << modifierNamesCount << " ModifierNames\n";
    std::cout << std::dec << networkedStringTablesCount << " NetworkedStringTables\n";
    std::cout << std::dec << weaponSettingsCount << " WeaponSettings\n";


    std::cout << "\n[Miscellaneous]\n";
    const std::string signatures[][2] = {

                        {"ClientState",               "? ? ? E8 ${'} 48 81 C4 68 04 00 00 C3"},
                        {"GlobalVars",                "48 8B 01 48 8D 15 ${'} FF 50 10 85 C0 75"},                      //488B01 488D15${'} FF5010 85C0 75
                        {"HighlightSettings",         "? 48 8B 15 ${'} 48 03 D1 4C 8D 04 40 ?"},                        //488B15${'} 4803D1 4C8D0440
                        {"InputSystem",               "00 00 00 00 89 4C 24 20 48 8D 0D ${'}"},                         //00000000 894C2420 488D0D$'
                        {"LocalPlayer",               "? 48 8B 05 ${???????? '} 488D0D???? 4488????? 4C89"},            //488B05${'} 488D0D???? 4488????? 4C89
                        {"NetworkVarTablePtr",        "? 48 8D 15 ${'} 83 7C CA 0C 07 74 ? 48 8B D7"},
                        {"SignonState",               "C6411401 803D?? ???74 68803D? ???? 7509833D ${? '}"},
                        {"WeaponSettingsMeta_base",   "? ? 4C 8B 83 ? ? ? ? 4C 8D 8B u4"},
                        {"LevelName",                 "? 48 8D 15 ${'} 48 8B 45 F8 48 8D 0D ?"},                        //488D15${'} 488B45F8488D0D
                        {"LocalEntityHandle",         "? ? 8B 05 ${'} 44 0F B7 05 ? ? ? ? 83 F8 FF 74"},                //8B05${'} 440FB705???? 83F8FF 74
                        {"cl_entitylist",             "81 F9 ? ? ? ? 7C % 85 C9 79 % 48 63 C1 48 8D 0D ${'}"},          //81F9u4 7C% 85C9 79% 4863C1 488D0D$'
                        {"NameList",                  "? 48 63 43 38 48 8D 0D ${'} 48 8D 04 40 48 8B 44 C1 E8 ? ? ?"},  //48634338 488D0D${'} 488D0440 488B44C1E8
                      //{"m_iName",                   "?  48 8D 8E u4                  49 8B D3 E8 ? ? ? ?  85 C0 74 21"},
                        {"m_iName",                   "37 48 8D 8E u4       48 8D 15 ? ?  ?  ?  E8 ? ? ? ?  85 C0 ?  ? "},
//2025Aug26        C3950F02 E908C383 000000CA 74F68548 8E8D4837    00000479 F7158D48   E800F9B2    00B63252 0875C085    E904588D 000000A6 @ 84321a
//2025Sep16        C3950F02 E908C383 000000CA 74F68548 8E8D4837    00000479 CF158D48   E800FB9F    00B7FDCA 0875C085    E904588D 000000A6 @ 84f762
                        {"m_iTeamNum",                "? ? 8B 87 u4 89 87 ? ? ? ? 33 C0 8B 15 ? ?"},
                        {"m_vecAbsOrigin",            "F3 0F 7E 97 u4"},
                        {"m_vecAbsVelocity",          "F3 0F 10 9F u4 F3 0F 10 97 ? ? ? ? F3 0F 59 DB"},
                        {"camera_origin",             "? ? 48 8B F9 0F 2E 89 u4 7A ? ? ?"},                             //488BF9 0F2E89u4 7A
                        {"ViewAngles",                "F2 0F 10 B6 u4 F3 41 0F 58 D3 ? ? ?"},
                      //{"m_iHealth",                 "? 44 8B 83 u4       45 85 C0 44 0F 48 C7 41 8B D0 2B 93 ? ? ? ?  85 D2 ? ? "},
//2025Aug26        0F00001D 1DF88111 110F0000 001E0889 838B4400   00000324 44C08545    41C7480F    932BD08B    00001AE0 307ED285   33C0570F @ 89e433
                        {"m_iHealth",                 "? ? 8B 88  u4       E8 ? ? ?    FF 89 44 24 ? 48 8B 84  24 ? ? ? ? 8B 80 E0"},
//2025Sep16        0060B900 A4F30000 8B48D233 00F02484 888B0000   00000324 993DF3E8    244489FF    848B4830    0000F024 E0808B00   8B00001A @ 8b1c50
//2025Oct07        0060B900 A4F30000 8B48D233 00F02484 888B0000   00000324 994413E8    244489FF    848B4830    0000F024 E0808B00   8B00001A @ 8b1e50
//2025Nov05        0060B900 A4F30000 8B48D233 02202484 888B0000   00000324 97DD03E8    244489FF    848B483C    00022024 E0808B00   8B00001A @ 8d06b0
                      //{"m_shieldHealth",            "? ? 39 87 u4       0F 84 ? ?  ? ? 48 8B 07 48 8B CF FF 90 ?  ?  ? ? 44 8B 97 ? ? ?"},
                        {"m_shieldHealth",            "? ? 39 87 u4       0F 84 ? 01 ? ? 48 8B 07 48 8B CF FF 90 ?  05 ? ? ?  8B"},
//2025Aug26        01A8878B 8D4C0000 20B0DE3D 0006BB00 87390000  000001A0 017E840F   8B480000  CF8B4807    05B090FF    8B4C0000  2DE0C205 @ 7d2cd5
//2025Sep16        01A8878B 8D4C0000 218EC03D 0006BB00 87390000  000001A0 0180840F   8B480000  CF8B4807    05B090FF    8B440000  0001A097 @ 7de9a3
//2025Nov05        01A8878B 8D4C0000 21FBC03D 0006BB00 87390000  000001A0 0190840F   8B480000  CF8B4807    05C890FF    8B440000  0001A097 @ 7fb183
                      //{"m_bleedoutState",           "?  ?  39 B8 u4       0F 45 F1 48 8B 03 48 8B CB FF 90 ?"},
//2025Aug26        FFCB8B48 0005B090 B60F4000 0001B9F6 B8390000    00002828 48F1450F    8B48038B    6090FFCB   4C000009 8D4CC88B 946C9C15 @ 89e50b
                        {"m_bleedoutState",           "74 28 8B 87 u4"},
//2025Aug26        246C8948 ED324040 050890FF C0840000 878B2874    00002828 F883C8FF 40057701 16EB01B5 44E48545 8B48C38B 950F41CF @ a5528a
//2025Sep16        246C8948 ED324040 050890FF C0840000 878B2874    00002830 F883C8FF 40057701 16EB01B5 44E48545 8B48C38B 950F41CF @ a6ff84
//2025Oct07        246C8948 ED324040 050890FF C0840000 878B2874    00002830 F883C8FF 40057701 16EB01B5 44E48545 8B48C38B 950F41CF @ a70184
//2025Nov05        246C8948 ED324040 051890FF C0840000 878B2874    00002920 F883C8FF 40057701 16EB01B5 44E48545 8B48C38B 950F41CF @ a93f14
                        {"m_latestNonOffhandWeapons", "? ? ? 48 0F BE 94 07 u4 80 FA FF ?"},
                        {"lastVisibleTime",           "? ? ? F3 41 0F 10 81 u4 66 3B 77 4E 0F 83 ? ? ? ? 4C 8B 57 30 45 0F B7 62 ? 66 45 85 E4 0F 84 ? ? ?"},
                        {"m_pStudioHdr",              "? 48 8B 9F u4 48 85 DB 75 ? 48 8D 0D"},                          //488B9Fu4 4885DB 75? 488D0D
                      //{"m_nForceBone",              "? ?  89 81 u4       C3 CC 48 89 6C 24 ? ?"},
                        {"m_nForceBone",              "? 4C 89 93 u4       66 89 B3 ?  0D ? ? C7"},
//2025Aug26        000FD8B3 B3884000 000015BD 385570E8 93894C00   00000DA0 90B38966    C700000D  000DFC83 FFFFFF00 894466FF 000D5093 @ 7a4f9f
//2025Sep16        000FE0B3 B3884000 000015C5 399A86E8 93894C00   00000DA8 98B38966    C700000D  000E0483 FFFFFF00 894466FF 000D5093 @ 7afd09
//2025Oct07        000FE0B3 B3884000 000015C5 399A86E8 93894C00   00000DA8 98B38966    C700000D  000E0483 FFFFFF00 894466FF 000D5093 @ 7aff09
                        {"ViewRender",                "? ? ? 0F 85 ? ? ? ? 48 8B 0D ${'} 48 8B 01 FF 50 20 48 8B 0D"},  //0F 85 ? ? ? ? 48 8B 0D ${'} 48 8B 01 FF 50 20 48 8B 0D
                        {"ViewMatrix",                "? ? 48 89 ? ? ? ? ? 49 8B B5 u4 49 8B 9D ? ? ? ? E8"},           //4889[5] 498BB5u4 498B9D[4] E8
                        {"ObserverList",              "? 48 8B 0D ${'}     48 85 C9 74 ? 48 8B 01 FF ? ? 48 85 C0 74 ? 48 63 4E 38"},
//2025Aug26        059C3DEA 247C8948 DB9E3840 74000041 0D8B4863   059C5DF6 74C98548    018B4857   483050FF  4C74C085   384E6348    C8048D48 @ 8213ea
//2025Sep16        05A5587A 247C8948 EB9E3840 74000041 0D8B485C   05A57886 74C98548    018B4850   483050FF  4574C085   384E6348    54C8848B @ 82d3ba
                      //{"m_gameTimescale",           "? C8 48 05 u4       ? ? ? ?  83 F8 ? ?"},
//2025Aug26        50FF018B C0854830 63484C74 8D48384E 0548C804   00000954 008B2074 74FFF883  F8B70F19 05E7C148 74FD0348 10E8C10D @ 821408
                        {"m_gameTimescale",           "? 8B 84 C8 u4                83 F8 ? ?"},
//2025Sep16        8B485074 3050FF01 74C08548 4E634845 C8848B38   00000954 74FFF883 F8B70F19  05E7C148 74FD0348 10E8C10D 75084739 @ 82d3d5

    };
    for (int i = 0; i < sizeof(signatures)/sizeof(signatures[0]); i++) {
        resume = 0;
        saveAddr[0] = 0;
        if (scanForPattern(resume, dataVirtualAddress, signatures[i][1], save, saveAddr)) {
            printf("%08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X @ %x\n",
                *(uint32_t*)(-16+ memoryBytes + resume),                   *(uint32_t*)(-12+ memoryBytes + resume),
                *(uint32_t*)(-8 + memoryBytes + resume),                   *(uint32_t*)(-4 + memoryBytes + resume),
                *(uint32_t*)(   + memoryBytes + resume),                   *(uint32_t*)( 4 + memoryBytes + resume),
                *(uint32_t*)( 8 + memoryBytes + resume),                   *(uint32_t*)(12 + memoryBytes + resume),
                *(uint32_t*)(16 + memoryBytes + resume),                   *(uint32_t*)(20 + memoryBytes + resume),
                *(uint32_t*)(24 + memoryBytes + resume),                   *(uint32_t*)(28 + memoryBytes + resume),
                resume);
            printf("%08X %08X %08X %08X %08X %08X %08X %08X\n",
                *(uint32_t*)(   + memoryBytes + resume),                   *(uint32_t*)( 4 + memoryBytes + resume),
                *(uint32_t*)( 8 + memoryBytes + resume),                   *(uint32_t*)(12 + memoryBytes + resume),
                *(uint32_t*)(16 + memoryBytes + resume),                   *(uint32_t*)(20 + memoryBytes + resume),
                *(uint32_t*)(24 + memoryBytes + resume),                   *(uint32_t*)(28 + memoryBytes + resume));
            if (saveAddr[0] > 0) for (int j = 0; j < (saveAddr[0] - resume) / 4; j++) fprintf(stderr, "         ");
            printf("%08X=%08X\n", *(uint32_t*)(memoryBytes + saveAddr[0]), *(uint32_t*)(memoryBytes + saveAddr[0]) + saveAddr[0] + 4);
            printf("         %08X %08X %08X %08X %08X %08X %08X %08X\n",
                *(uint32_t*)(   + memoryBytes + resume) + saveAddr[0] + 4, *(uint32_t*)( 4 + memoryBytes + resume) + saveAddr[0] + 4,
                *(uint32_t*)( 8 + memoryBytes + resume) + saveAddr[0] + 4, *(uint32_t*)(12 + memoryBytes + resume) + saveAddr[0] + 4,
                *(uint32_t*)(16 + memoryBytes + resume) + saveAddr[0] + 4, *(uint32_t*)(20 + memoryBytes + resume) + saveAddr[0] + 4,
                *(uint32_t*)(24 + memoryBytes + resume) + saveAddr[0] + 4, *(uint32_t*)(28 + memoryBytes + resume) + saveAddr[0] + 4);
            printf("%s\n", signatures[i][1].c_str());
            //if (signatures[i][0] == "LocalPlayer") save[0] += 8;
            std::cout << signatures[i][0] << "=0x" << std::hex << save[0] << "\n";
            std::string fullName = formStr("[.Miscellaneous]" + std::string(signatures[i][0]));
            if (mapOffsets[fullName] == 0)
                mapOffsets[fullName] = save[0];
        } else
            std::cout << signatures[i][0] << "=0x........\n";
    }


    std::ofstream fileOffsets("_offsets.ini");
    int offsetsCount = 0;
    for (auto i = mapOffsets.cbegin(); i != mapOffsets.cend(); ++i) {
        fileOffsets << i->first << " " << std::hex << i->second << "\n";
        offsetsCount++;
    }
    fileOffsets.close();
    std::cout << "\n" << std::dec << offsetsCount << " Offsets\n";


  /*struct RawConCommand
    {
        uint64_t m_vTable;                // 0x0000
        uint64_t m_pNext;                 // 0x0008
      //bool     m_bRegistered;           // 0x0010
      //char     pad_0011[7];             // 0x0011
        uint64_t m_pszName;               // 0x0018
        uint64_t m_pszDescription;        // 0x0020
        uint64_t m_pszDataType;           // 0x0028
        uint32_t m_iFlags;                // 0x0030
        char     pad_0034[20];            // 0x0034
        uint64_t m_fnCommandCallback;     // 0x0048
        uint64_t m_fnCompletionCallback;  // 0x0050
        uint32_t m_fnCommandType;         // 0x0058
    };*/
    maxStep = dataVirtualAddress + dataVirtualSize;
    for (size_t step = dataVirtualAddress; step < maxStep;) {
        char nameConCommand[128] = {0};
        uint64_t addressConCommand;
        resume = step;
        //if (scanForPattern(resume, maxStep, "u4 ? 7F 00 00 ? ? ? ? ? 7F 00 00 ? ? ? ? ? 7F 00 00 ? ? ? ? ? 7F 00 00", save, saveAddr)) {
        if (scanForPattern(resume, maxStep, "u4 ? 7F 00 00 ? ? ? ? ? 7F 00 00 ? ? ? ? ? 7F 00 00", save, saveAddr)) {
            addressConCommand = *(uint64_t*)(memoryBytes + resume);
            addressConCommand -= g_base;
            if (addressConCommand < memoryBytesSize) {
                uint64_t m_pszName           = *(uint64_t*)(memoryBytes + addressConCommand + 0x0018 - 8);
                uint64_t m_fnCommandCallback = *(uint64_t*)(memoryBytes + addressConCommand + 0x0048 - 8);
                uint32_t m_fnCommandType     = *(uint32_t*)(memoryBytes + addressConCommand + 0x0058 - 8);
                m_pszName -= g_base;
                if (m_pszName < memoryBytesSize && m_fnCommandCallback && m_fnCommandType == 2) {
                    std::memcpy(nameConCommand, &memoryBytes[m_pszName], 31);
                    if (nameConCommand[0] == '+') {
                        size_t temp = m_fnCommandCallback - g_base;
                        if (scanForPattern(temp, temp + 256, "84 C0 75 44 8B 05 ${'} 3B D8 74 3A 8B 0D ? ? ? ? 3B D9 74 30 85 C0 75 08", save, saveAddr)) {
                            std::string fullName = formStr("[Buttons]" + std::string(nameConCommand));
                            if (mapButtons[fullName] == 0)
                                mapButtons[fullName] = save[0];
                        }
                    }
                }
            }
            step = resume + 8;
        } else { break; }
    }
    std::ofstream fileButtons("_buttons.ini");
    int buttonsCount = 0;
    for (auto i = mapButtons.cbegin(); i != mapButtons.cend(); ++i) {
        fileButtons << i->first << " " << std::hex << i->second << "\n";
        buttonsCount++;
    }
    fileButtons.close();
    std::cout << "\n" << std::dec << buttonsCount << " Buttons\n";


    return 0;
}
