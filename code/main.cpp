#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#define MAX_COOKIES_COUNT_DEFAULT 512
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

union cookie
{
    char *Parts[7];
    struct
    {
        char *Name;
        char *Value;
        char *Domain;
        char *Path;
        char *Expiration;
        char *Size;
        char *HttpOnly;
    };
};

struct cookies
{
    int32_t CookieCount;
    int32_t MaxCookieCount;
    union cookie *Cookies;
};

static int32_t
CompareFunc(const void *Left, const void *Right)
{
    union cookie *A = (union cookie *)Left;
    union cookie *B = (union cookie *)Right;
    return(strcmp(A->Domain, B->Domain));
}

inline void
ScanToDelimiter(char **Ptr, char C)
{
    char *At = *Ptr;
    while(*At &&
          At[0] != C)
    {
        ++At;
    }
    *Ptr = At;
}

int main(int32_t argc, char **argv)
{
    if(argc < 2)
    {
        printf("Usage: %s [cookies file] [count (opt)]\n", argv[0]);
        return(-1);
    }

    FILE *File = fopen(argv[1], "rb");
    if(!File)
    {
        fprintf(stderr, "Failed to open file %s\n", argv[1]);
        return(-2);
    }

    fseek(File, 0, SEEK_END);
    int32_t FileSize = ftell(File);
    fseek(File, 0, SEEK_SET);
    
    char *Contents = (char *)malloc(sizeof(*Contents) * (FileSize + 1));
    if(!Contents)
    {
        fprintf(stderr, "Failed to allocate space for file\n");
        return(-3);
    }

    fread(Contents, sizeof(*Contents), FileSize, File);
    Contents[FileSize] = 0;

    struct cookies Cookies = {0};
    Cookies.MaxCookieCount = ((argc == 3) ? atoi(argv[2]) : MAX_COOKIES_COUNT_DEFAULT);
    Cookies.Cookies = (union cookie *)malloc(sizeof(Cookies.Cookies) * Cookies.MaxCookieCount);

    char *At = Contents;
    while(*At)
    {

        union cookie *Cookie = Cookies.Cookies + Cookies.CookieCount++;
        for(int32_t Part = 0; Part < ArrayCount(Cookie->Parts); ++Part)
        {
            Cookie->Parts[Part] = At;
            ScanToDelimiter(&At, '\t');
            *At = 0;
            ++At;
        }
        ScanToDelimiter(&At, '\n');
        ++At;
    }

    qsort(Cookies.Cookies, Cookies.CookieCount, sizeof(Cookies.Cookies[0]), CompareFunc);

    printf("# Netscape HTTP Cookie File\n\n");
    for(int32_t CookieIndex = 0; CookieIndex < Cookies.CookieCount; ++CookieIndex)
    {
        union cookie *Cookie = Cookies.Cookies + CookieIndex;
        if(Cookie->Name)
        {
            // TODO(rick): There seems to be a discrepency in the number of
            // seconds of about 9 hours compared to other tools, I need to
            // investigate what the correct calcuations are. For now this works
            // and I haven't seen a problem with using the resulting cookie
            // file.
            time_t Seconds = 0;
            if(strcmp(Cookie->Expiration, "Session") == 0)
            {
                Seconds = time(0);
            }
            else
            {
                tm Time = {0};
                Time.tm_isdst = 0;
                sscanf(Cookie->Expiration, "%d-%d-%dT%d:%d:%dZ",
                       &Time.tm_year, &Time.tm_mon, &Time.tm_mday,
                       &Time.tm_hour, &Time.tm_min, &Time.tm_sec);
                Time.tm_year -= 1900; // NOTE(rick): Current year minus 1900
                Time.tm_mon -= 1;  // NOTE(rick): January = 0
                Seconds = mktime(&Time);
            }

            printf("%s%s\t%s\t%s\t%s\t%lld\t%s\t%s\n",
                   Cookie->Domain[0] == '.' ? "" : ".",
                   Cookie->Domain,
                   "TRUE",
                   Cookie->Path,
                   (Cookie->HttpOnly[0] ? "TRUE" : "FALSE"),
                   Seconds,
                   Cookie->Name,
                   Cookie->Value);
        }
    }

    fclose(File);
    return(0);
}
