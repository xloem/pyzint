#include <Python.h>
#include "endianness.h"
#include "src/zint/backend/zint.h"

extern void make_html_friendly(unsigned char * string, char * html_version);

typedef struct {
    PyObject_HEAD
    struct zint_symbol *symbol;
    PyObject *data;
    char *human_symbology;
    char *buffer;
    Py_ssize_t length;
} CZINT;


static PyObject *
CZINT_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    CZINT *self;

    self = (CZINT *) type->tp_alloc(type, 0);
    return (PyObject *) self;
}

static void
CZINT_dealloc(CZINT *self) {
    if (self->symbol != NULL) {
        ZBarcode_Delete(self->symbol);
        self->symbol = NULL;
    }
    Py_CLEAR(self->data);
    self->buffer = NULL;

    Py_TYPE(self)->tp_free((PyObject *) self);
}

uint8_t octet2char(const unsigned char* src) {
    unsigned char result = 0;
    result |= (src[0]?1:0) << 7;
    result |= (src[1]?1:0) << 6;
    result |= (src[2]?1:0) << 5;
    result |= (src[3]?1:0) << 4;
    result |= (src[4]?1:0) << 3;
    result |= (src[5]?1:0) << 2;
    result |= (src[6]?1:0) << 1;
    result |= (src[7]?1:0);

    return result;
}

static int
CZINT_init(CZINT *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"data", "kind", "scale", NULL};

    self->symbol = ZBarcode_Create();

    if (self->symbol == NULL) {
        PyErr_Format(
            PyExc_RuntimeError,
            "Symbol initialization failed"
        );
        return -1;
    }

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "Ob|f", kwlist,
            &self->data, &self->symbol->symbology, &self->symbol->scale
    )) return -1;

    switch (self->symbol->symbology) {
        case (BARCODE_CODE11):
            self->human_symbology = "code11";
            break;
        case (BARCODE_C25MATRIX):
            self->human_symbology = "c25matrix";
            break;
        case (BARCODE_C25INTER):
            self->human_symbology = "c25inter";
            break;
        case (BARCODE_C25IATA):
            self->human_symbology = "c25iata";
            break;
        case (BARCODE_C25LOGIC):
            self->human_symbology = "c25logic";
            break;
        case (BARCODE_C25IND):
            self->human_symbology = "c25ind";
            break;
        case (BARCODE_CODE39):
            self->human_symbology = "code39";
            break;
        case (BARCODE_EXCODE39):
            self->human_symbology = "excode39";
            break;
        case (BARCODE_EANX):
            self->human_symbology = "eanx";
            break;
        case (BARCODE_EANX_CHK):
            self->human_symbology = "eanx_chk";
            break;
        case (BARCODE_EAN128):
            self->human_symbology = "ean128";
            break;
        case (BARCODE_CODABAR):
            self->human_symbology = "codabar";
            break;
        case (BARCODE_CODE128):
            self->human_symbology = "code128";
            break;
        case (BARCODE_DPLEIT):
            self->human_symbology = "dpleit";
            break;
        case (BARCODE_DPIDENT):
            self->human_symbology = "dpident";
            break;
        case (BARCODE_CODE16K):
            self->human_symbology = "code16k";
            break;
        case (BARCODE_CODE49):
            self->human_symbology = "code49";
            break;
        case (BARCODE_CODE93):
            self->human_symbology = "code93";
            break;
        case (BARCODE_FLAT):
            self->human_symbology = "flat";
            break;
        case (BARCODE_RSS14):
            self->human_symbology = "rss14";
            break;
        case (BARCODE_RSS_LTD):
            self->human_symbology = "rss_ltd";
            break;
        case (BARCODE_RSS_EXP):
            self->human_symbology = "rss_exp";
            break;
        case (BARCODE_TELEPEN):
            self->human_symbology = "telepen";
            break;
        case (BARCODE_UPCA):
            self->human_symbology = "upca";
            break;
        case (BARCODE_UPCA_CHK):
            self->human_symbology = "upca_chk";
            break;
        case (BARCODE_UPCE):
            self->human_symbology = "upce";
            break;
        case (BARCODE_UPCE_CHK):
            self->human_symbology = "upce_chk";
            break;
        case (BARCODE_POSTNET):
            self->human_symbology = "postnet";
            break;
        case (BARCODE_MSI_PLESSEY):
            self->human_symbology = "msi_plessey";
            break;
        case (BARCODE_FIM):
            self->human_symbology = "fim";
            break;
        case (BARCODE_LOGMARS):
            self->human_symbology = "logmars";
            break;
        case (BARCODE_PHARMA):
            self->human_symbology = "pharma";
            break;
        case (BARCODE_PZN):
            self->human_symbology = "pzn";
            break;
        case (BARCODE_PHARMA_TWO):
            self->human_symbology = "pharma_two";
            break;
        case (BARCODE_PDF417):
            self->human_symbology = "pdf417";
            break;
        case (BARCODE_PDF417TRUNC):
            self->human_symbology = "pdf417trunc";
            break;
        case (BARCODE_MAXICODE):
            self->human_symbology = "maxicode";
            break;
        case (BARCODE_QRCODE):
            self->human_symbology = "qrcode";
            break;
        case (BARCODE_CODE128B):
            self->human_symbology = "code128b";
            break;
        case (BARCODE_AUSPOST):
            self->human_symbology = "auspost";
            break;
        case (BARCODE_AUSREPLY):
            self->human_symbology = "ausreply";
            break;
        case (BARCODE_AUSROUTE):
            self->human_symbology = "ausroute";
            break;
        case (BARCODE_AUSREDIRECT):
            self->human_symbology = "ausredirect";
            break;
        case (BARCODE_ISBNX):
            self->human_symbology = "isbnx";
            break;
        case (BARCODE_RM4SCC):
            self->human_symbology = "rm4scc";
            break;
        case (BARCODE_DATAMATRIX):
            self->human_symbology = "datamatrix";
            break;
        case (BARCODE_EAN14):
            self->human_symbology = "ean14";
            break;
        case (BARCODE_VIN):
            self->human_symbology = "vin";
            break;
        case (BARCODE_CODABLOCKF):
            self->human_symbology = "codablockf";
            break;
        case (BARCODE_NVE18):
            self->human_symbology = "nve18";
            break;
        case (BARCODE_JAPANPOST):
            self->human_symbology = "japanpost";
            break;
        case (BARCODE_KOREAPOST):
            self->human_symbology = "koreapost";
            break;
        case (BARCODE_RSS14STACK):
            self->human_symbology = "rss14stack";
            break;
        case (BARCODE_RSS14STACK_OMNI):
            self->human_symbology = "rss14stack_omni";
            break;
        case (BARCODE_RSS_EXPSTACK):
            self->human_symbology = "rss_expstack";
            break;
        case (BARCODE_PLANET):
            self->human_symbology = "planet";
            break;
        case (BARCODE_MICROPDF417):
            self->human_symbology = "micropdf417";
            break;
        case (BARCODE_ONECODE):
            self->human_symbology = "onecode";
            break;
        case (BARCODE_PLESSEY):
            self->human_symbology = "plessey";
            break;
        case (BARCODE_TELEPEN_NUM):
            self->human_symbology = "telepen_num";
            break;
        case (BARCODE_ITF14):
            self->human_symbology = "itf14";
            break;
        case (BARCODE_KIX):
            self->human_symbology = "kix";
            break;
        case (BARCODE_AZTEC):
            self->human_symbology = "aztec";
            break;
        case (BARCODE_DAFT):
            self->human_symbology = "daft";
            break;
        case (BARCODE_MICROQR):
            self->human_symbology = "microqr";
            break;
        case (BARCODE_HIBC_128):
            self->human_symbology = "hibc_128";
            break;
        case (BARCODE_HIBC_39):
            self->human_symbology = "hibc_39";
            break;
        case (BARCODE_HIBC_DM):
            self->human_symbology = "hibc_dm";
            break;
        case (BARCODE_HIBC_QR):
            self->human_symbology = "hibc_qr";
            break;
        case (BARCODE_HIBC_PDF):
            self->human_symbology = "hibc_pdf";
            break;
        case (BARCODE_HIBC_MICPDF):
            self->human_symbology = "hibc_micpdf";
            break;
        case (BARCODE_HIBC_BLOCKF):
            self->human_symbology = "hibc_blockf";
            break;
        case (BARCODE_HIBC_AZTEC):
            self->human_symbology = "hibc_aztec";
            break;
        case (BARCODE_DOTCODE):
            self->human_symbology = "dotcode";
            break;
        case (BARCODE_HANXIN):
            self->human_symbology = "hanxin";
            break;
        case (BARCODE_MAILMARK):
            self->human_symbology = "mailmark";
            break;
        case (BARCODE_AZRUNE):
            self->human_symbology = "azrune";
            break;
        case (BARCODE_CODE32):
            self->human_symbology = "code32";
            break;
        case (BARCODE_EANX_CC):
            self->human_symbology = "eanx_cc";
            break;
        case (BARCODE_EAN128_CC):
            self->human_symbology = "ean128_cc";
            break;
        case (BARCODE_RSS14_CC):
            self->human_symbology = "rss14_cc";
            break;
        case (BARCODE_RSS_LTD_CC):
            self->human_symbology = "rss_ltd_cc";
            break;
        case (BARCODE_RSS_EXP_CC):
            self->human_symbology = "rss_exp_cc";
            break;
        case (BARCODE_UPCA_CC):
            self->human_symbology = "upca_cc";
            break;
        case (BARCODE_UPCE_CC):
            self->human_symbology = "upce_cc";
            break;
        case (BARCODE_RSS14STACK_CC):
            self->human_symbology = "rss14stack_cc";
            break;
        case (BARCODE_RSS14_OMNI_CC):
            self->human_symbology = "rss14_omni_cc";
            break;
        case (BARCODE_RSS_EXPSTACK_CC):
            self->human_symbology = "rss_expstack_cc";
            break;
        case (BARCODE_CHANNEL):
            self->human_symbology = "channel";
            break;
        case (BARCODE_CODEONE):
            self->human_symbology = "codeone";
            break;
        case (BARCODE_GRIDMATRIX):
            self->human_symbology = "gridmatrix";
            break;
        case (BARCODE_UPNQR):
            self->human_symbology = "upnqr";
            break;
        case (BARCODE_ULTRA):
            self->human_symbology = "ultra";
            break;
        case (BARCODE_RMQR):
            self->human_symbology = "rmqr";
            break;
        default:
            PyErr_Format(
                PyExc_ValueError,
                "Unknown barcode type %d",
                self->symbol->symbology
            );
            return -1;
    }

    if (PyBytes_Check(self->data)) {
        if (PyBytes_AsStringAndSize(self->data, &self->buffer, &self->length) == -1) {
            return -1;
        }
    } else if (PyUnicode_Check(self->data)) {
        self->buffer = (char *)PyUnicode_AsUTF8AndSize(self->data, &self->length);
        if (self->buffer == NULL) {
            return -1;
        }
    } else {
        PyErr_SetNone(PyExc_ValueError);
        return -1;
    }

    Py_INCREF(self->data);
    return 0;
}

static PyObject* CZINT_repr(CZINT *self) {
    if (self->symbol == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Pool not initialized");
        return NULL;
    }
    return PyUnicode_FromFormat(
        "<%s as %p: kind=%d buffer=%s (%d)>",
        Py_TYPE(self)->tp_name, self, self->symbol->symbology, self->buffer, self->length
    );
}

int parse_color_hex(const char *str, unsigned int *target) {
    if (str == NULL) {
        return 0;
    }
    if (str[0] != '#') {
        PyErr_SetFormat(
            PyExc_ValueError,
            "Invalid color: %s. Color must be started with '#'",
            str
        );
        return -1;
    }

    int res = sscanf(&str[1],"%2x%2x%2x", &target[0], &target[1], &target[2]);
    if (res < 0) {
        PyErr_SetString(
            PyExc_ValueError,
            "Invalid color format. Color must be in '#ffffff'"
        );
        return -1;
    }

    return 0;
}

int parse_color_str(const char *str, char *target) {
    if (str == NULL) {
        return 0;
    }
    if (str[0] != '#') {
        PyErr_Format(
            PyExc_ValueError,
            "Invalid color: %s. Color must be started with '#'",
            str
        );
        return -1;
    }
    memcpy(target, &str[1], 6);
    return 0;
}


PyDoc_STRVAR(CZINT_render_bmp_docstring,
    "Render bmp.\n\n"
    "    ZBarcode('data').render_bmp(angle: int = 0) -> bytes"
);
static PyObject* CZINT_render_bmp(
    CZINT *self, PyObject *args, PyObject *kwds
) {
    static char *kwlist[] = {"angle", "fgcolor", "bgcolor", NULL};

    int angle = 0;
    unsigned int fgcolor[3] = {0, 0, 0};
    unsigned int bgcolor[3] = {255, 255, 255};

    char *fgcolor_str = NULL;
    char *bgcolor_str = NULL;


    if (!PyArg_ParseTupleAndKeywords(
        args, kwds, "|iss", kwlist,
        &angle, &fgcolor_str, &bgcolor_str
    )) return NULL;

    if (parse_color_hex(fgcolor_str, (unsigned int *)&fgcolor)) return NULL;
    if (parse_color_hex(bgcolor_str, (unsigned int *)&bgcolor)) return NULL;

    int res = 0;
    char *bmp = NULL;
    int bmp_1bit_size = 0;
    Py_BEGIN_ALLOW_THREADS

    res = ZBarcode_Encode_and_Buffer(
        self->symbol,
        (unsigned char *)self->buffer,
        self->length, angle
    );

    unsigned int width = self->symbol->bitmap_width;
    unsigned int height = self->symbol->bitmap_height;

    unsigned char bitmap[height][width + 8];
    memset(&bitmap, 0, height * (width + 8));

    for (unsigned int i=0; i<height*width; i++) {
        bitmap[i/width][i%width] = self->symbol->bitmap[i * 3];
    }

    static const unsigned int header_size = 62;
    const int bmp_1bit_with_bytes = (width / 8 + (width % 8 == 0?0:1));
    const int padding = ((4 - (width * 3) % 4) % 4);
    bmp_1bit_size = (
        header_size + bmp_1bit_with_bytes * height + (height * padding)
    );

    static const unsigned char bmp_template[header_size] = {
      0x42, 0x4d,
      0x00, 0x00, 0x00, 0x00, // size
      0x00, 0x00, 0x00, 0x00, // padding (zero)
      0x3e, 0x00, 0x00, 0x00, // 62
      0x28, 0x00, 0x00, 0x00, // 40
      0x00, 0x00, 0x00, 0x00, // width
      0x00, 0x00, 0x00, 0x00, // height
      0x01, 0x00, 0x01, 0x00, // planes and bpp
      0x00, 0x00, 0x00, 0x00, // compression
      0x00, 0x00, 0x00, 0x00, // size
      0xc4, 0x0e, 0x00, 0x00, // x pxls per meter
      0xc4, 0x0e, 0x00, 0x00, // y pxls per meter
      0x02, 0x00, 0x00, 0x00, // colors in table
      0x02, 0x00, 0x00, 0x00, // important color in table
      0x00, 0x00, 0x00, 0x00, // red channel - fgcolor
      0xff, 0xff, 0xff, 0xff  // green channel - bgcolor
    };

    if (res == 0) {
        bmp = calloc(bmp_1bit_size, sizeof(char *));

        memcpy(bmp, &bmp_template, header_size);

        unsigned int be_value = hton32(bmp_1bit_size);
        bmp[5] = (unsigned char)(be_value);
        bmp[4] = (unsigned char)(be_value >> 8);
        bmp[3] = (unsigned char)(be_value >> 16);
        bmp[2] = (unsigned char)(be_value >> 24);

        be_value = hton32(width);
        bmp[21] = (unsigned char)(be_value);
        bmp[20] = (unsigned char)(be_value >> 8);
        bmp[19] = (unsigned char)(be_value >> 16);
        bmp[18] = (unsigned char)(be_value >> 24);

        be_value = hton32(height);
        bmp[25] = (unsigned char)(be_value);
        bmp[24] = (unsigned char)(be_value >> 8);
        bmp[23] = (unsigned char)(be_value >> 16);
        bmp[22] = (unsigned char)(be_value >> 24);

        bmp[54] = (unsigned char)fgcolor[0];
        bmp[55] = (unsigned char)fgcolor[1];
        bmp[56] = (unsigned char)fgcolor[2];

        bmp[58] = (unsigned char)bgcolor[0];
        bmp[59] = (unsigned char)bgcolor[1];
        bmp[60] = (unsigned char)bgcolor[2];

        char *pixels = &bmp[header_size];

        unsigned int offset = 0;
        for(unsigned int y=height-1; y > 0; --y) {
            for(unsigned int x=0; x < width; x+=8) {
                pixels[offset] = octet2char(&bitmap[y][x]);
                offset++;
            }
            offset += padding;
        }
    }

    Py_END_ALLOW_THREADS

    if (res > 0) {
        PyErr_Format(
            PyExc_RuntimeError,
            "Error while rendering: %s",
            self->symbol->errtxt
        );
        return NULL;
    }
    PyObject *result = PyBytes_FromStringAndSize(bmp, bmp_1bit_size);
    if (result == NULL) {
        free(bmp);
        return NULL;
    }
    free(bmp);
    return result;
}

PyDoc_STRVAR(CZINT_render_svg_docstring,
    "Render bmp.\n\n"
    "    ZBarcode('data').render_svg(angle: int = 0) -> bytes"
);
static PyObject* CZINT_render_svg(
    CZINT *self, PyObject *args, PyObject *kwds
) {
    static char *kwlist[] = {"angle", "fgcolor", "bgcolor", NULL};

    int angle = 0;
    char *fgcolor_str = "#000000";
    char *bgcolor_str = "#FFFFFF";


    if (!PyArg_ParseTupleAndKeywords(
        args, kwds, "|iss", kwlist,
        &angle, &fgcolor_str, &bgcolor_str
    )) return NULL;

    if (parse_color_str(fgcolor_str, (char *)&self->symbol->fgcolour)) return NULL;
    if (parse_color_str(bgcolor_str, (char *)&self->symbol->bgcolour)) return NULL;

    int res = 0;
    char *fsvg = NULL;
    int len_fsvg = 0;
    int max_len = 4 * 1024 * 1024;
    struct zint_vector_rect *rect;
    struct zint_vector_hexagon *hex;
    struct zint_vector_circle *circle;
    struct zint_vector_string *string;
    float ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy;
    float radius;


    Py_BEGIN_ALLOW_THREADS

    res = ZBarcode_Encode_and_Buffer_Vector(self->symbol, (unsigned char *)self->buffer, self->length, angle);

    int html_len = strlen((char *)self->symbol->text) + 1;
    for (unsigned int i = 0; i < strlen((char *)self->symbol->text); i++) {
        switch(self->symbol->text[i]) {
            case '>':
            case '<':
            case '"':
            case '&':
            case '\'':
                html_len += 6;
                break;
        }
    }
    char html_string[html_len];

    fsvg = calloc(max_len, sizeof(char *));

    /* Start writing the header */
    len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "<?xml version=\"1.0\" standalone=\"no\"?>\n");

    len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n");
    len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "   \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");
    len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "<svg width=\"%d\" height=\"%d\" version=\"1.1\"\n", (int) ceil(self->symbol->vector->width), (int) ceil(self->symbol->vector->height));
    len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "   xmlns=\"http://www.w3.org/2000/svg\">\n");
    len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "   <desc>Zint Generated Symbol\n");
    len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "   </desc>\n");
    len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "\n   <g id=\"barcode\" fill=\"#%s\">\n", self->symbol->fgcolour);
    len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "      <rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"#%s\" />\n", (int) ceil(self->symbol->vector->width), (int) ceil(self->symbol->vector->height), self->symbol->bgcolour);
    rect = self->symbol->vector->rectangles;
    while (rect) {
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "      <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" />\n", rect->x, rect->y, rect->width, rect->height);
        rect = rect->next;
    }

    hex = self->symbol->vector->hexagons;
    while (hex) {
        radius = hex->diameter / 2.0;
        ay = hex->y + (1.0 * radius);
        by = hex->y + (0.5 * radius);
        cy = hex->y - (0.5 * radius);
        dy = hex->y - (1.0 * radius);
        ey = hex->y - (0.5 * radius);
        fy = hex->y + (0.5 * radius);
        ax = hex->x;
        bx = hex->x + (0.86 * radius);
        cx = hex->x + (0.86 * radius);
        dx = hex->x;
        ex = hex->x - (0.86 * radius);
        fx = hex->x - (0.86 * radius);
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "      <path d=\"M %.2f %.2f L %.2f %.2f L %.2f %.2f L %.2f %.2f L %.2f %.2f L %.2f %.2f Z\" />\n", ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy);
        hex = hex->next;
    }

    circle = self->symbol->vector->circles;
    while (circle) {
        if (circle->colour) {
            len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "      <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" fill=\"#%s\" />\n", circle->x, circle->y, circle->diameter / 2.0, self->symbol->bgcolour);
        } else {
            len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "      <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" fill=\"#%s\" />\n", circle->x, circle->y, circle->diameter / 2.0, self->symbol->fgcolour);
        }
        circle = circle->next;
    }

    string = self->symbol->vector->strings;
    while (string) {
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "      <text x=\"%.2f\" y=\"%.2f\" text-anchor=\"middle\"\n", string->x, string->y);
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "         font-family=\"Helvetica\" font-size=\"%.1f\" fill=\"#%s\" >\n", string->fsize, self->symbol->fgcolour);
        make_html_friendly(string->text, html_string);
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "         %s\n", html_string);
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "      </text>\n");
        string = string->next;
    }

    len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "   </g>\n");
    len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "</svg>\n");

    Py_END_ALLOW_THREADS

    if (res > 0) {
        PyErr_Format(
            PyExc_RuntimeError,
            "Error while rendering: %s",
            self->symbol->errtxt
        );
        return NULL;
    }
    PyObject *result = PyBytes_FromStringAndSize(fsvg, len_fsvg);
    if (result == NULL) {
        free(fsvg);
        return NULL;
    }
    free(fsvg);
    return result;
}


static PyMethodDef CZINT_methods[] = {
    {
        "render_bmp",
        (PyCFunction) CZINT_render_bmp, METH_VARARGS | METH_KEYWORDS,
        CZINT_render_bmp_docstring
    },
    {
        "render_svg",
        (PyCFunction) CZINT_render_svg, METH_VARARGS | METH_KEYWORDS,
        CZINT_render_svg_docstring
    },
    {NULL}  /* Sentinel */

};



static PyTypeObject
ZINTType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Zint",
    .tp_doc = "Pyzint - c-binding to zint",
    .tp_basicsize = sizeof(CZINT),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = CZINT_new,
    .tp_init = (initproc) CZINT_init,
    .tp_dealloc = (destructor) CZINT_dealloc,
    .tp_methods = CZINT_methods,
    .tp_repr = (reprfunc) CZINT_repr
};


static PyModuleDef pyzint_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pyzint",
    .m_doc = "Pyzint binding",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_pyzint(void) {
    static PyTypeObject* ZINTTypeP = &ZINTType;
    PyEval_InitThreads();

    PyObject *m;

    m = PyModule_Create(&pyzint_module);

    if (m == NULL) return NULL;

    if (PyType_Ready(ZINTTypeP) < 0) return NULL;

    Py_INCREF(ZINTTypeP);

    if (PyModule_AddObject(m, "Zint", (PyObject *) ZINTTypeP) < 0) {
        Py_XDECREF(ZINTTypeP);
        Py_XDECREF(m);
        return NULL;
    }

    PyModule_AddIntConstant(m, "BARCODE_CODE11", BARCODE_CODE11);
    PyModule_AddIntConstant(m, "BARCODE_C25MATRIX", BARCODE_C25MATRIX);
    PyModule_AddIntConstant(m, "BARCODE_C25INTER", BARCODE_C25INTER);
    PyModule_AddIntConstant(m, "BARCODE_C25IATA", BARCODE_C25IATA);
    PyModule_AddIntConstant(m, "BARCODE_C25LOGIC", BARCODE_C25LOGIC);
    PyModule_AddIntConstant(m, "BARCODE_C25IND", BARCODE_C25IND);
    PyModule_AddIntConstant(m, "BARCODE_CODE39", BARCODE_CODE39);
    PyModule_AddIntConstant(m, "BARCODE_EXCODE39", BARCODE_EXCODE39);
    PyModule_AddIntConstant(m, "BARCODE_EANX", BARCODE_EANX);
    PyModule_AddIntConstant(m, "BARCODE_EANX_CHK", BARCODE_EANX_CHK);
    PyModule_AddIntConstant(m, "BARCODE_EAN128", BARCODE_EAN128);
    PyModule_AddIntConstant(m, "BARCODE_CODABAR", BARCODE_CODABAR);
    PyModule_AddIntConstant(m, "BARCODE_CODE128", BARCODE_CODE128);
    PyModule_AddIntConstant(m, "BARCODE_DPLEIT", BARCODE_DPLEIT);
    PyModule_AddIntConstant(m, "BARCODE_DPIDENT", BARCODE_DPIDENT);
    PyModule_AddIntConstant(m, "BARCODE_CODE16K", BARCODE_CODE16K);
    PyModule_AddIntConstant(m, "BARCODE_CODE49", BARCODE_CODE49);
    PyModule_AddIntConstant(m, "BARCODE_CODE93", BARCODE_CODE93);
    PyModule_AddIntConstant(m, "BARCODE_FLAT", BARCODE_FLAT);
    PyModule_AddIntConstant(m, "BARCODE_RSS14", BARCODE_RSS14);
    PyModule_AddIntConstant(m, "BARCODE_RSS_LTD", BARCODE_RSS_LTD);
    PyModule_AddIntConstant(m, "BARCODE_RSS_EXP", BARCODE_RSS_EXP);
    PyModule_AddIntConstant(m, "BARCODE_TELEPEN", BARCODE_TELEPEN);
    PyModule_AddIntConstant(m, "BARCODE_UPCA", BARCODE_UPCA);
    PyModule_AddIntConstant(m, "BARCODE_UPCA_CHK", BARCODE_UPCA_CHK);
    PyModule_AddIntConstant(m, "BARCODE_UPCE", BARCODE_UPCE);
    PyModule_AddIntConstant(m, "BARCODE_UPCE_CHK", BARCODE_UPCE_CHK);
    PyModule_AddIntConstant(m, "BARCODE_POSTNET", BARCODE_POSTNET);
    PyModule_AddIntConstant(m, "BARCODE_MSI_PLESSEY", BARCODE_MSI_PLESSEY);
    PyModule_AddIntConstant(m, "BARCODE_FIM", BARCODE_FIM);
    PyModule_AddIntConstant(m, "BARCODE_LOGMARS", BARCODE_LOGMARS);
    PyModule_AddIntConstant(m, "BARCODE_PHARMA", BARCODE_PHARMA);
    PyModule_AddIntConstant(m, "BARCODE_PZN", BARCODE_PZN);
    PyModule_AddIntConstant(m, "BARCODE_PHARMA_TWO", BARCODE_PHARMA_TWO);
    PyModule_AddIntConstant(m, "BARCODE_PDF417", BARCODE_PDF417);
    PyModule_AddIntConstant(m, "BARCODE_PDF417TRUNC", BARCODE_PDF417TRUNC);
    PyModule_AddIntConstant(m, "BARCODE_MAXICODE", BARCODE_MAXICODE);
    PyModule_AddIntConstant(m, "BARCODE_QRCODE", BARCODE_QRCODE);
    PyModule_AddIntConstant(m, "BARCODE_CODE128B", BARCODE_CODE128B);
    PyModule_AddIntConstant(m, "BARCODE_AUSPOST", BARCODE_AUSPOST);
    PyModule_AddIntConstant(m, "BARCODE_AUSREPLY", BARCODE_AUSREPLY);
    PyModule_AddIntConstant(m, "BARCODE_AUSROUTE", BARCODE_AUSROUTE);
    PyModule_AddIntConstant(m, "BARCODE_AUSREDIRECT", BARCODE_AUSREDIRECT);
    PyModule_AddIntConstant(m, "BARCODE_ISBNX", BARCODE_ISBNX);
    PyModule_AddIntConstant(m, "BARCODE_RM4SCC", BARCODE_RM4SCC);
    PyModule_AddIntConstant(m, "BARCODE_DATAMATRIX", BARCODE_DATAMATRIX);
    PyModule_AddIntConstant(m, "BARCODE_EAN14", BARCODE_EAN14);
    PyModule_AddIntConstant(m, "BARCODE_VIN", BARCODE_VIN);
    PyModule_AddIntConstant(m, "BARCODE_CODABLOCKF", BARCODE_CODABLOCKF);
    PyModule_AddIntConstant(m, "BARCODE_NVE18", BARCODE_NVE18);
    PyModule_AddIntConstant(m, "BARCODE_JAPANPOST", BARCODE_JAPANPOST);
    PyModule_AddIntConstant(m, "BARCODE_KOREAPOST", BARCODE_KOREAPOST);
    PyModule_AddIntConstant(m, "BARCODE_RSS14STACK", BARCODE_RSS14STACK);
    PyModule_AddIntConstant(m, "BARCODE_RSS14STACK_OMNI", BARCODE_RSS14STACK_OMNI);
    PyModule_AddIntConstant(m, "BARCODE_RSS_EXPSTACK", BARCODE_RSS_EXPSTACK);
    PyModule_AddIntConstant(m, "BARCODE_PLANET", BARCODE_PLANET);
    PyModule_AddIntConstant(m, "BARCODE_MICROPDF417", BARCODE_MICROPDF417);
    PyModule_AddIntConstant(m, "BARCODE_ONECODE", BARCODE_ONECODE);
    PyModule_AddIntConstant(m, "BARCODE_PLESSEY", BARCODE_PLESSEY);
    PyModule_AddIntConstant(m, "BARCODE_TELEPEN_NUM", BARCODE_TELEPEN_NUM);
    PyModule_AddIntConstant(m, "BARCODE_ITF14", BARCODE_ITF14);
    PyModule_AddIntConstant(m, "BARCODE_KIX", BARCODE_KIX);
    PyModule_AddIntConstant(m, "BARCODE_AZTEC", BARCODE_AZTEC);
    PyModule_AddIntConstant(m, "BARCODE_DAFT", BARCODE_DAFT);
    PyModule_AddIntConstant(m, "BARCODE_MICROQR", BARCODE_MICROQR);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_128", BARCODE_HIBC_128);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_39", BARCODE_HIBC_39);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_DM", BARCODE_HIBC_DM);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_QR", BARCODE_HIBC_QR);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_PDF", BARCODE_HIBC_PDF);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_MICPDF", BARCODE_HIBC_MICPDF);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_BLOCKF", BARCODE_HIBC_BLOCKF);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_AZTEC", BARCODE_HIBC_AZTEC);
    PyModule_AddIntConstant(m, "BARCODE_DOTCODE", BARCODE_DOTCODE);
    PyModule_AddIntConstant(m, "BARCODE_HANXIN", BARCODE_HANXIN);
    PyModule_AddIntConstant(m, "BARCODE_MAILMARK", BARCODE_MAILMARK);
    PyModule_AddIntConstant(m, "BARCODE_AZRUNE", BARCODE_AZRUNE);
    PyModule_AddIntConstant(m, "BARCODE_CODE32", BARCODE_CODE32);
    PyModule_AddIntConstant(m, "BARCODE_EANX_CC", BARCODE_EANX_CC);
    PyModule_AddIntConstant(m, "BARCODE_EAN128_CC", BARCODE_EAN128_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS14_CC", BARCODE_RSS14_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS_LTD_CC", BARCODE_RSS_LTD_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS_EXP_CC", BARCODE_RSS_EXP_CC);
    PyModule_AddIntConstant(m, "BARCODE_UPCA_CC", BARCODE_UPCA_CC);
    PyModule_AddIntConstant(m, "BARCODE_UPCE_CC", BARCODE_UPCE_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS14STACK_CC", BARCODE_RSS14STACK_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS14_OMNI_CC", BARCODE_RSS14_OMNI_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS_EXPSTACK_CC", BARCODE_RSS_EXPSTACK_CC);
    PyModule_AddIntConstant(m, "BARCODE_CHANNEL", BARCODE_CHANNEL);
    PyModule_AddIntConstant(m, "BARCODE_CODEONE", BARCODE_CODEONE);
    PyModule_AddIntConstant(m, "BARCODE_GRIDMATRIX", BARCODE_GRIDMATRIX);
    PyModule_AddIntConstant(m, "BARCODE_UPNQR", BARCODE_UPNQR);
    PyModule_AddIntConstant(m, "BARCODE_ULTRA", BARCODE_ULTRA);
    PyModule_AddIntConstant(m, "BARCODE_RMQR", BARCODE_RMQR);
    return m;
}