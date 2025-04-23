#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "cons/type.h"
#include "cons/macro.h"

#include "cons/error.h"

#include "cons/buffer.h"
#include "cons/file.h"

#define RIFF_SIGNATURE IDENTIFIER_TO_U32('R','I','F','F')
#define PAL__SIGNATURE IDENTIFIER_TO_U32('P','A','L',' ')
#define DATA_SIGNATURE IDENTIFIER_TO_U32('d','a','t','a')

// https://github.com/mgba-emu/mgba/tree/master/src/util/image/export.c#L11
typedef struct MPalRIFF {
    u32 riffSignature; // Compare to RIFF_SIGNATURE
    u32 fileSize; // Excludes riffSignature and fileSize
    u32 palSignature; // Compare to PAL__SIGNATURE
    u32 dataSignature; // Compare to DATA_SIGNATURE
    u32 dataSize; // Excludes dataSignature and dataSize
    u16 _unk14; // ???
    u16 entryCount; // Color count.
    u32 entries[0]; // Colors in RGBA.
} MPalRIFF;

#define TO_BGR555(x)                       \
      (((((x) >> 0) & 0xFF) >> 3) << 0 )   \
    | (((((x) >> 8) & 0xFF) >> 3) << 5 )   \
    | (((((x) >> 16) & 0xFF) >> 3) << 10)

// https://github.com/arthurtilly/rhythmtengoku/tree/master/include/graphics.h#L95
typedef struct TengokuPal {
    u16 entries[0]; // Colors in BGR555. See TO_BGR555
} TengokuPal;

int main(int argc, char** argv) {
    if (argc < 2) {
        printf(
            "tnmg - Convert MGBA palettes (.pal files) to Tengoku binary palettes\n"
            "Usage: %s <input_palette.pal> [output_palette.tgk]\n",
            argv[0]
        );
        return 1;
    }

    const char* inputPath = argv[1];

    bool madeOutputPath = false;
    char* outputPath = argv[2];
    if (argc < 3) {
        madeOutputPath = true;
        outputPath = (char*)malloc(strlen(inputPath) + STR_LIT_LEN(".tgk"));
        sprintf(outputPath, "%s.tgk", inputPath);
    }

    printf("MGBA palette at '%s' -> Tengoku palette at '%s'\n", inputPath, outputPath);

    ConsBuffer mgbaPaletteBuf = LoadWholeFile(inputPath);
    if (!BufferIsValid(&mgbaPaletteBuf))
        Panic("Failed to load MGBA palette from path %s", inputPath);
    
    MPalRIFF* mgbaPalette = mgbaPaletteBuf.data_void;
    if (mgbaPalette->riffSignature != RIFF_SIGNATURE)
        Panic("MGBA palette RIFF signature is nonmatching");
    if (mgbaPalette->palSignature != PAL__SIGNATURE)
        Panic("MGBA palette PAL signature is nonmatching");
    if (mgbaPalette->dataSignature != DATA_SIGNATURE)
        Panic("MGBA palette DATA signature is nonmatching");

    ConsBuffer tengokuPaletteBuf;
    BufferInit(&tengokuPaletteBuf, mgbaPalette->entryCount * sizeof(u16));

    TengokuPal* tengokuPalette = tengokuPaletteBuf.data_void;

    for (u32 i = 0; i < mgbaPalette->entryCount; i++) {
        tengokuPalette->entries[i] = TO_BGR555((mgbaPalette->entries[i]) & 0xFFFFFF);
    }

    BufferDestroy(&mgbaPaletteBuf);

    if (WriteWholeFile(&tengokuPaletteBuf, outputPath))
        Panic("Failed to write tengoku palette to path %s", outputPath);
    
    BufferDestroy(&tengokuPaletteBuf);

    if (madeOutputPath)
        free(outputPath);

    printf("\nAll done.\n");
    return 0;
}
