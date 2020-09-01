#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define OFFSETBITS 7
#define LENGTHBITS (11-OFFSETBITS)

#define OFFSETMASK ((1 << (OFFSETBITS)) - 1)
#define LENGTHMASK ((1 << (LENGTHBITS)) - 1)
#define OFFSETLENGTH(x,y) (x << LENGTHBITS | y)

// dominique-nique-nique
// sir_sid_eastman
// abracadabraaaa
// cabeacadabeaeeaeead
// bcaadddccacacac
// AXABRACADABRARRARRAX

char *sourceFilename = "C:\\Users\\Kübra USLUCA\\Desktop\\source.txt";
char *encodedFilename = "C:\\Users\\Kübra USLUCA\\Desktop\\encoded.txt";

struct token
{
    //int offset_len;
    uint8_t  offset;
    uint8_t  length;
    char next;

};

// huffman
typedef struct node
{
    char ch;
    int freq;
    struct node *left;
    struct node *right;

} node;

node * dugum[100];
int dugumSize=0;
int toplamBit;
int toplamBinaryBit;
int toplamKarakterBit;
int toplamFrekansBit;

// iki string'in ilk kaç karakteri özdes (limit sayisi kadar kontrol yapar)
int string_eslesme(char *s1, char *s2, int limit)
{
    int len = 0;

    while (*s1++ == *s2++ && len < limit)
        len++;

    return len;
}

int string_uzaklik(char *s1, char *s2, int limit)
{
    ptrdiff_t bytes  = 0;

    while (*s1++ == *s2++ && bytes < limit)

        bytes = ((char *)s2) - ((char *)s1);

    return bytes;
}

struct token *encode(char *text, int limit, int *numTokens)
{
    int yer = 1 << 3;   // hafizada kac tokenlik yer ayirdigimiz
    int _numTokens = 0;     // Kac token olusturdugumuz
    struct token t;     // Olusturulacak token
    char *ileri_tampon, *arama_tampon;       // buffer

    struct token *encoded = malloc(yer * sizeof(struct token));     // token icin yer ayiriyoruz.

    // token olusturma dongusu
    for (ileri_tampon = text; ileri_tampon < text + limit; ileri_tampon++)
    {
        int len1 = 0;
        t.offset = len1;
        // arama tamponunu ileri tamponun 31 (OFFSETMASK) karakter gerisine koyuyoruz.
        arama_tampon = ileri_tampon - OFFSETMASK;

        if (arama_tampon < text)  // arama tamponunun metnin disina cikmasina engel
            arama_tampon = text;

        int max_len = 0;    // arama tamponunda bulunan en uzun kelimenin (eslesen) boyutu

        char *max_match = ileri_tampon;    // arama tamponunda bulunan en uzun kelimenin (eslesen) konumu

        // arama tamponu icinde arama yap.
        for (; arama_tampon < ileri_tampon; arama_tampon++)
        {

            int len = string_eslesme(arama_tampon, ileri_tampon, LENGTHMASK);

            if (len > max_len)
            {
                max_len = len;
                max_match = arama_tampon;

                int len1 = string_uzaklik(arama_tampon, ileri_tampon, LENGTHMASK);
                t.offset = len1;
            }
        }
        // Metnin son karakteri eslemeye dahilse eslemeyi kisaltiyoruz
        // (6,11,NULL) yerine (6,10,e)
        if (ileri_tampon + max_len >= text + limit)
        {
            max_len = text + limit - ileri_tampon;
        }

        // bulunan eslesmeye göre token olusturma

        //t.offset_len = OFFSETLENGTH(ileri_tampon - max_match, max_len);

        t.length = max_len;
        ileri_tampon += max_len;

        t.next = *ileri_tampon;
        //printf("\n ----- %d", max_len);

        // gerekirse, hafizada yer aç
        if (_numTokens + 1 > yer)
        {
            yer = yer << 1;
            encoded = realloc(encoded, yer * sizeof(struct token));
        }

        encoded[_numTokens++] = t;  // oluþturulan tokeni, array'e kaydet.
    }

    if (numTokens)
        *numTokens = _numTokens;

    FILE *f;
    f = fopen(encodedFilename, "wb");

    fwrite(t.next, sizeof(struct token), 1, f);
    /*
    fwrite(&t[1], sizeof(struct token), 1, f);
    fwrite(&t[2], sizeof(struct token), 1, f);
    fwrite(&t[3], sizeof(struct token), 1, f);
    fwrite(&t[4], sizeof(struct token), 1, f);
    */

    return encoded;
}


// huffman
void dugumEkle(node * element)
{
    dugumSize++;
    dugum[dugumSize] = element;
    int now = dugumSize;

    while(dugum[now/2] -> freq > element -> freq)
    {
        dugum[now] = dugum[now/2];
        now /= 2;
    }

    dugum[now] = element;
}

node * dugumSil()
{
    node * minElement,*lastElement;
    int child,now;
    minElement = dugum[1];
    lastElement = dugum[dugumSize--];
    for(now = 1; now*2 <= dugumSize ; now = child)
    {
        child = now*2;
        if(child != dugumSize && dugum[child+1]->freq < dugum[child] -> freq )
        {
            child++;
        }
        if(lastElement -> freq > dugum[child] -> freq)
        {
            dugum[now] = dugum[child];
        }
        else
        {
            break;
        }
    }
    dugum[now] = lastElement;
    return minElement;
}

void bitHesapla(int a)
{
    toplamBinaryBit += a ;

    //printf("\n%d\n", toplamBit);
}


void yazdir(node *temp,char *code)
{
    if(temp->left==NULL && temp->right==NULL)
    {
        printf("\n    %c : %s\n",temp->ch,code);

        // Bit hesabii
        printf("\nFrekans = %d\n", temp->freq);
        printf("Bit Sayisi = %d\n", strlen(code));

        int i = 0;

        int a = (temp->freq) * (strlen(code)) ;
        printf("Frekans x Bit Sayisi = %d\n", a);
        bitHesapla(a);

        return;
    }
    int length = strlen(code);
    char leftcode[10],rightcode[10];
    strcpy(leftcode,code);
    strcpy(rightcode,code);
    leftcode[length] = '0';
    leftcode[length+1] = '\0';
    rightcode[length] = '1';
    rightcode[length+1] = '\0';
    yazdir(temp->left,leftcode);
    yazdir(temp->right,rightcode);
}


void huffman(char *kaynak_metin, int length_huffman)
{
    char *arr;
    arr = kaynak_metin;
    int n = 0;


    int fr[length_huffman];
    int visited = -1;

    for(int i = 0; i < length_huffman; i++)
    {
        int count = 1;
        for(int j = i+1; j < length_huffman; j++)
        {
            if(arr[i] == arr[j])
            {
                count++;
                fr[j] = visited;
            }
        }
        if(fr[i] != visited)
            fr[i] = count;
    }

    char ch;
    int freq,i;

    dugum[0] = (node *)malloc(sizeof(node));
    dugum[0]->freq = 0;


    printf("\n---------------------\n");
    printf(" Karakter | Frekansi\n");
    printf("---------------------\n");
    for(int i = 0; i < length_huffman; i++)
    {
        if(fr[i] != visited)
        {
            printf("    %c", arr[i]);
            printf("    |  ");
            printf("  %d\n", fr[i]);

            toplamFrekansBit += fr[i];

            // huffman

            ch = arr[i];
            freq = fr[i];
            node * temp = (node *) malloc(sizeof(node));
            temp -> ch = ch;
            temp -> freq = freq;
            temp -> left = temp -> right = NULL;
            dugumEkle(temp);

            n++;
        }
    }
    printf("\---------------------\n");
    printf("\nBenzersiz Karakter Sayisi: %d\n", n);

    // n x 8 --> toplam karakter bitini verecek

    toplamKarakterBit = n * 8 ;

    if(n==1)
    {
        printf("\n    %c : 0\n",ch);
        toplamBinaryBit += 1 ;
        return 0;
    }
    for(i=0; i<n-1 ; i++)
    {
        node * left = dugumSil();
        node * right = dugumSil();
        node * temp = (node *) malloc(sizeof(node));
        temp -> ch = 0;
        temp -> left = left;
        temp -> right = right;
        temp -> freq = left->freq + right -> freq;
        dugumEkle(temp);
    }
    node *tree = dugumSil();
    char code[10];
    code[0] = '\0';
    yazdir(tree,code);

}


char *file_read(FILE *f, int *size) // dosyanin tamamini tek seferde okuyoruz
{
    char *buffer;
    fseek(f, 0L, SEEK_END);
    *size = ftell(f);
    buffer = (char*)calloc(size, sizeof(char));
    fseek(f, 0L, SEEK_SET);
    fread(buffer, sizeof(char), *size, f);
    return buffer;
}

int main(void)
{
    FILE *f;
    int metin_boyutu = 1, token_sayisi;
    char *kaynak_metin = "";
    struct token *encoded_metin;


    int choice;

    do
    {
        printf("\n\n\n\t\t\t\tPROGRAMLAMA-2\n\n");
        printf("\t\t\t\t1. LZ77\n");
        printf("\t\t\t\t2. DEFLATE \n");
        printf("\t\t\t\t3. Karsilastir \n");
        printf("\t\t\t\t4. Exit\n");

        printf("\t\t\t\tIslem Seciniz ---> ");
        scanf("%d",&choice);
        printf("\n\n\n");

        switch(choice)
        {
        case 1:

            if (f = fopen(sourceFilename, "rb"))
            {
                kaynak_metin = file_read(f, &metin_boyutu);
                fclose(f);
            }


            //printf("\n");
            printf(kaynak_metin);

            encoded_metin = encode(kaynak_metin, (metin_boyutu+1), &token_sayisi);
            //printf("\n\n%d\n", token_sayisi);

            if (f = fopen(encodedFilename, "wb"))
            {
                int i;
                for (i = 0; i < token_sayisi; i++)
                {

                    if ( encoded_metin[i].offset == 0 && encoded_metin[i].length == 0 && i == (token_sayisi-1))
                    {
                        token_sayisi--;
                        continue;
                    }
                    else
                    {
                        printf("\n<%d, %d, C(%c)>\n", encoded_metin[i].offset, encoded_metin[i].length, encoded_metin[i].next);
                        fwrite(&encoded_metin[i], sizeof(struct token), 1, f);

                        // <0, 0, C(a)>  dosya gosterimi
                        // fprintf(f,"\n%d",encoded_metin[i].offset);
                        // fprintf(f," %d",encoded_metin[i].length);
                        // fprintf(f," C(%c)\n",encoded_metin[i].next);
                    }

                }

                fclose(f);
            }

            printf("\nOrjinal Boyut: %d \nEncode Boyutu: %d \n", metin_boyutu, token_sayisi * sizeof(struct token));

            break;
        case 2:

            // Deflate Algoritmasi

            // Huffman

            printf("\n------Sadece Huffman Algoritmasi Yapabildik. LZSS Algoritmasini Huffman'a Uyarlayamadik.\n\n");
            if (f = fopen(sourceFilename, "rb"))
            {
                kaynak_metin = file_read(f, &metin_boyutu);
                fclose(f);
            }


            //printf("\n");
            printf(kaynak_metin);
            printf("\n");

            int length_huffman = strlen(kaynak_metin);
            huffman(kaynak_metin, length_huffman);

            printf("\n\nToplam Binary Biti = %d\n", toplamBinaryBit);
            printf("Toplam Karakter Biti = %d\n", toplamKarakterBit);
            printf("Toplam Frekans Biti = %d\n", toplamFrekansBit);

            toplamBit = toplamBinaryBit + toplamKarakterBit + toplamFrekansBit ;
            printf("Toplam Bit = %d\n", toplamBit);

            printf("\nNormal Boyutu = %d bit, %d bayt", (length_huffman * 8), length_huffman);
            printf("\nEncode Boyutu = %d bit, %.3f bayt\n", toplamBit, ((float)toplamBit/8.0));



            break;
        case 3:

            // Iki algoritmanin boyutunu yaz...

            printf("\nLZ77 Orjinal Boyut: %d bayt\nLZ77 Encode Boyutu: %d bayt\n", metin_boyutu, token_sayisi * sizeof(struct token));

            printf("\nDEFLATE Normal Boyutu = %d bit, %d bayt", (length_huffman * 8), length_huffman);
            printf("\nDEFLATE Encode Boyutu = %d bit, %.3f bayt\n", toplamBit, ((float)toplamBit/8.0));

            break;
        case 4:

            printf("Program Sonlandiriliyor...!\n");
            exit(0);
            break;
        default:
            printf("Gecersiz Secim...!\n");
            break;
        }

    }
    while (choice != 4);


    return 0;
}
