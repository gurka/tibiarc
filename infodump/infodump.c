/*
 * Copyright 2024 "Simon Sandström"
 * Copyright 2024 "John Högberg"
 *
 * This file is part of tibiarc.
 *
 * tibiarc is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * tibiarc is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with tibiarc. If not, see <https://www.gnu.org/licenses/>.
 */

#include "playback.h"
#include "rendering.h"

#ifdef _WIN32
#    define DIR_SEP "\\"
#else
#    define DIR_SEP "/"
#endif

void load_files(uintptr_t recording_data,
                int recording_length,
                uintptr_t pic_data,
                int pic_length,
                uintptr_t spr_data,
                int spr_length,
                uintptr_t dat_data,
                int dat_length) {
    if (playback_loaded) {
        playback_Free(&playback);
    }

    struct trc_data_reader recording;
    recording.Position = 0;
    recording.Data = (const uint8_t *)recording_data;
    recording.Length = recording_length;

    struct trc_data_reader pic;
    pic.Position = 0;
    pic.Data = (const uint8_t *)pic_data;
    pic.Length = pic_length;

    struct trc_data_reader spr;
    spr.Position = 0;
    spr.Data = (const uint8_t *)spr_data;
    spr.Length = spr_length;

    struct trc_data_reader dat;
    dat.Position = 0;
    dat.Data = (const uint8_t *)dat_data;
    dat.Length = dat_length;

    playback_Init(&playback, "", &recording, 0, 0, 0, &pic, &spr, &dat);

    free((void *)pic_data);
    free((void *)spr_data);
    free((void *)dat_data);

    playback_loaded = true;
}

struct player_data {
    struct memory_file PictureFile;
    struct memory_file SpriteFile;
    struct memory_file TypeFile;

    struct trc_data_reader Pictures;
    struct trc_data_reader Sprites;
    struct trc_data_reader Types;
};

static bool player_OpenData(const char *dataFolder, struct player_data *data) {
    char picturePath[FILENAME_MAX];
    char spritePath[FILENAME_MAX];
    char typePath[FILENAME_MAX];

    if (snprintf(picturePath,
                 FILENAME_MAX,
                 "%s%s",
                 dataFolder,
                 DIR_SEP "Tibia.pic") < 0) {
        (void)fprintf(stderr, "Could not merge data path with Tibia.pic\n");
        return false;
    }

    if (snprintf(spritePath,
                 FILENAME_MAX,
                 "%s%s",
                 dataFolder,
                 DIR_SEP "Tibia.spr") < 0) {
        (void)fprintf(stderr, "Could not merge data path with Tibia.spr\n");
        return false;
    }

    if (snprintf(typePath,
                 FILENAME_MAX,
                 "%s%s",
                 dataFolder,
                 DIR_SEP "Tibia.dat") < 0) {
        (void)fprintf(stderr, "Could not merge data path with Tibia.dat\n");
        return false;
    }

    if (!memoryfile_Open(picturePath, &data->PictureFile)) {
        (void)fprintf(stderr, "Failed to open Tibia.pic\n");
        return false;
    } else {
        if (!memoryfile_Open(spritePath, &data->SpriteFile)) {
            (void)fprintf(stderr, "Failed to open Tibia.spr\n");
            return false;
        } else {
            if (!memoryfile_Open(typePath, &data->TypeFile)) {
                (void)fprintf(stderr, "Failed to open Tibia.dat\n");
                return false;
            } else {
                data->Pictures.Data = data->PictureFile.View;
                data->Pictures.Length = data->PictureFile.Size;
                data->Pictures.Position = 0;

                data->Sprites.Data = data->SpriteFile.View;
                data->Sprites.Length = data->SpriteFile.Size;
                data->Sprites.Position = 0;

                data->Types.Data = data->TypeFile.View;
                data->Types.Length = data->TypeFile.Size;
                data->Types.Position = 0;

                return true;
            }

            memoryfile_Close(&data->SpriteFile);
        }

        memoryfile_Close(&data->PictureFile);
    }

    return false;
}

static void player_CloseData(struct player_data *data) {
    memoryfile_Close(&data->PictureFile);
    memoryfile_Close(&data->SpriteFile);
    memoryfile_Close(&data->TypeFile);
}

int main(int argc, char *argv[]) {
    int major = 0, minor = 0, preview = 0;

    if (argc < 3 || argc > 4) {
        fprintf(stderr, "usage: %s DATA_FOLDER RECORDING [VERSION]\n", argv[0]);
        return 1;
    }

    if (argc == 4) {
        if (sscanf(argv[3], "%u.%u.%u", &major, &minor, &preview) < 2) {
            fprintf(stderr,
                    "version must be in the format 'X.Y', e.g. '8.55'\n");
            return 1;
        }
    }

    trc_ChangeErrorReporting(TRC_ERROR_REPORT_MODE_TEXT);

    if (!rendering_Init(&rendering, 800, 600)) {
        return 1;
    }

    struct memory_file file_recording;
    struct player_data data;

    if (!player_OpenData(argv[1], &data) ||
        !memoryfile_Open(argv[2], &file_recording)) {
        return 1;
    }

    if (!playback_Init(&playback,
                       argv[2],
                       &(struct trc_data_reader){.Data = file_recording.View,
                                                 .Length = file_recording.Size,
                                                 .Position = 0},
                       major,
                       minor,
                       preview,
                       &data.Pictures,
                       &data.Sprites,
                       &data.Types)) {
        return 1;
    }

    player_CloseData(&data);

    while (true) {
        handle_input();
        playback_ProcessPackets(&playback);
        rendering_Render(&rendering, &playback);
    }

    playback_Free(&playback);
    rendering_Free(&rendering);

    memoryfile_Close(&file_recording);

    return 0;
}
