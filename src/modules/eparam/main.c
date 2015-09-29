//#include <nuttx/config.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <systemlib/param/param.h>

#include "eparam.h"

__EXPORT int eparam_main(int argc, char *argv[]);

static FILE *gFile = NULL;
static const char* factoryFileName = "/fs/microsd/factory.params";

static void do_save(void *arg, param_t param)
{
    const char *search_string = (const char*)arg;
    const char *name = (const char*)param_name(param);

    if (strncmp(search_string, name, strlen(search_string)) == 0)
    {
        const size_t value_size = param_size(param);

        void *param_value = malloc(value_size);

        // get param value
        if (param_value != NULL)
        {
            if (param_get(param, param_value))
            {
                printf("failed to get %s value\n", name);
                // failed to receive param value
                free(param_value);
                param_value = NULL;
            }
        }

        if (param_value != NULL)
        {
            // save name
            const size_t name_size = strlen(name) + 1; // including \0
            fwrite(&name_size, sizeof(name_size), 1, gFile);
            fwrite(name, name_size, 1, gFile);

            // save value
            fwrite(&value_size, sizeof(value_size), 1, gFile);

            fwrite(param_value, value_size, 1, gFile);
        }

        free(param_value);
    }
}

int eparam_save(const char *search_string, const char *filename, const char *mode)
{
    int result = 0;
    FILE *f = fopen(filename, mode);

    if (f == NULL)
    {
        printf("Can't open file %s\n", filename);
        result = -1;
    }

    if (result == 0)
    {
        gFile = f;
        param_foreach(do_save, (void*)search_string, 0);
        gFile = NULL;
    }

    if (f != NULL)
    {
        fclose(f);
    }

    return result;
}

int eparam_load(const char *filename)
{
    int result = 0;
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Can't open file %s\n", filename);
        result = -1;
    }

    while (result == 0)
    {
        size_t rc = 0;
        size_t size = 0;
        char *name = NULL;
        char *value = NULL;

        // read name
        rc = fread(&size, sizeof(size), 1, f);
        if (rc != 1)
        {
            if (feof(f))
            {
                // no more data
                break;
            }
            printf("fread failed. (%d %d)\n", rc, sizeof(size));
            result = -1;
        }

        if (result == 0)
        {
            name = (char*)malloc(size);
            if (name == NULL)
            {
                printf("malloc failed\n");
                result = -1;
            }
        }

        if (result == 0)
        {
            rc = fread(name, size, 1, f);
            if (rc != 1)
            {
                printf("fread failed\n");
                result = -1;
            }
        }

        // read value
        if (result == 0)
        {
            rc = fread(&size, sizeof(size), 1, f);
            if (rc != 1)
            {
                printf("fread failed\n");
                result = -1;
            }
        }

        if (result == 0)
        {
            value = (char*)malloc(size);
            if (value == NULL)
            {
                printf("malloc failed\n");
                result = -1;
            }
        }

        if (result == 0)
        {
            rc = fread(value, size, 1, f);
            if (rc != 1)
            {
                printf("fread failed\n");
                result = -1;
            }
        }

        // set param value
        if (result == 0)
        {
            param_t param = param_find(name);

            if (param != PARAM_INVALID)
            {
                if (param_set(param, value))
                {
                    printf("failed to set value for %s\n", name);
                }
            }
            else
            {
                printf("param %s not found\n", name);
            }
        }

        free(name);
        free(value);
    }

    if (f != NULL)
    {
        fclose(f);
    }

    return result;
}

int eparam_factorySave(void)
{
    eparam_save("SENS_ACC_CDATE", factoryFileName, "w");
    eparam_save("SENS_ACC_CTEMP", factoryFileName, "a");
    eparam_save("SENS_ACC_XOFF", factoryFileName, "a");
    eparam_save("SENS_ACC_XSCALE", factoryFileName, "a");
    eparam_save("SENS_ACC_YOFF", factoryFileName, "a");
    eparam_save("SENS_ACC_YSCALE", factoryFileName, "a");
    eparam_save("SENS_ACC_ZOFF", factoryFileName, "a");
    eparam_save("SENS_ACC_ZSCALE", factoryFileName, "a");
    eparam_save("SENS_BARO_QNH", factoryFileName, "a");
    eparam_save("SENS_GYRO_CDATE", factoryFileName, "a");
    eparam_save("SENS_GYRO_CTEMP", factoryFileName, "a");
    eparam_save("SENS_GYRO_XOFF", factoryFileName, "a");
    eparam_save("SENS_GYRO_XSCALE", factoryFileName, "a");
    eparam_save("SENS_GYRO_YOFF", factoryFileName, "a");
    eparam_save("SENS_GYRO_YSCALE", factoryFileName, "a");
    eparam_save("SENS_GYRO_ZOFF", factoryFileName, "a");
    eparam_save("SENS_GYRO_ZSCALE", factoryFileName, "a");
    eparam_save("SENS_MAG_CDATE", factoryFileName, "a");
    eparam_save("SENS_MAG_CTEMP", factoryFileName, "a");
    eparam_save("SENS_MAG_XCT_OFF", factoryFileName, "a");
    eparam_save("SENS_MAG_XOFF", factoryFileName, "a");
    eparam_save("SENS_MAG_XPECT_X", factoryFileName, "a");
    eparam_save("SENS_MAG_XPECT_Y", factoryFileName, "a");
    eparam_save("SENS_MAG_XPECT_Z", factoryFileName, "a");
    eparam_save("SENS_MAG_XSCALE", factoryFileName, "a");
    eparam_save("SENS_MAG_YOFF", factoryFileName, "a");
    eparam_save("SENS_MAG_YSCALE", factoryFileName, "a");
    eparam_save("SENS_MAG_ZOFF", factoryFileName, "a");
    eparam_save("SENS_MAG_ZSCALE", factoryFileName, "a");
    return 0;
}

int eparam_factoryLoad(void)
{
    return eparam_load(factoryFileName);
}

int eparam_factoryReset(void)
{
    // save params
    int SYS_ACT = 0;
    int A_DEVICE_ID = 0;
    int MAV_SYS_ID = 0;

    if (param_get(param_find("SYS_ACT"), &SYS_ACT))
    {
        printf("failed to get SYS_ACT value");
    }

    if (param_get(param_find("A_DEVICE_ID"), &A_DEVICE_ID))
    {
        printf("failed to get A_DEVICE_ID value");
    }

    if (param_get(param_find("MAV_SYS_ID"), &MAV_SYS_ID))
    {
        printf("failed to get MAV_SYS_ID value");
    }

    param_reset_all();

    eparam_load(factoryFileName);

    // restore params
    if (param_set(param_find("SYS_ACT"), &SYS_ACT))
    {
        printf("failed to set SYS_ACT value");
    }

    if (param_set(param_find("A_DEVICE_ID"), &A_DEVICE_ID))
    {
        printf("failed to set A_DEVICE_ID value");
    }

    if (param_set(param_find("MAV_SYS_ID"), &MAV_SYS_ID))
    {
        printf("failed to set MAV_SYS_ID value");
    }

    return 0;
}

int
eparam_main(int argc, char *argv[])
{
    if (argc >= 4 && strcmp(argv[1], "save") == 0)
    {
        int i = 0;
        const char *filename = argv[2];
        const char *search_string = argv[3];

        eparam_save(search_string, filename, "w");
        for (i = 4; i < argc; i++)
        {
            search_string = argv[i];
            eparam_save(search_string, filename, "a");
        }
    }
    else if (argc == 3 && strcmp(argv[1], "load") == 0)
    {
        const char *filename = argv[2];
        eparam_load(filename);
    }
    else if (argc == 2 && strcmp(argv[1], "factorySave") == 0)
    {
        eparam_factorySave();
    }
    else if (argc == 2 && strcmp(argv[1], "factoryLoad") == 0)
    {
        eparam_factoryLoad();
    }
    else if (argc == 2 && strcmp(argv[1], "factoryReset") == 0)
    {
        eparam_factoryReset();
    }
    else
    {
        printf("eparam save <filename> <search_string1> [<search_string2> ...]\n");
        printf("eparam load <filename>\n");
    }

    return 0;
}
