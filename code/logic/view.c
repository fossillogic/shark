/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/code/commands.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/* ---------------------------------------------------------------------
    Preloaded language/token tables (maximized for common languages)
    --------------------------------------------------------------------- */
static const char *KW_CODE[] = {
    // Control flow
    "if","else","for","while","do","switch","case","default",
    "break","continue","goto","return",

    // Types (C/C++)
    "void","char","short","int","long","float","double","signed","unsigned",
    "size_t","ssize_t","ptrdiff_t","struct","union","enum","typedef",
    "class","interface","fn","let","var","const","static","extern",
    "volatile","register","auto","inline","restrict","_Bool","_Complex","_Imaginary",

    // C++/Java
    "public","private","protected","virtual","override","final","abstract",
    "template","typename","namespace","using","friend","operator","explicit",
    "this","new","delete","throw","try","catch","finally","import","package",
    "extends","implements","instanceof","synchronized","transient","throws",

    // Modern languages
    "async","await","yield","lambda","import","from","package","module",
    "with","as","pass","raise","except","def","class","self","global","nonlocal",

    // Rust/Go
    "fn","pub","crate","impl","use","match","const","let","mut","enum","struct",
    "trait","type","where","ref","move","unsafe","dyn","super","Self","self",

    // JS / TS
    "function","export","extends","new","this","super","typeof","instanceof",
    "let","var","const","class","constructor","get","set","import","from",
    "await","async","yield","return","break","continue","switch","case","default",

    // Python
    "def","pass","class","self","with","as","try","except","finally","raise",
    "import","from","lambda","yield","global","nonlocal","assert","del","elif",
    "False","True","None","not","or","and","is","in",

    // Shell
    "if","then","fi","elif","else","for","while","do","done","case","esac",
    "function","select","until","in","break","continue","return","exit","export",
    "readonly","declare","local","typeset","eval","exec","set","unset","source",

    // HTML/CSS
    "html","head","body","div","span","script","style","link","meta","title",
    "table","tr","td","th","ul","ol","li","form","input","button","select",
    "option","textarea","label","fieldset","legend","header","footer","nav",
    "main","section","article","aside","h1","h2","h3","h4","h5","h6","br","hr",
    "class","id","src","href","alt","type","value","name","action","method",
    "checked","disabled","readonly","selected","placeholder","required",

    // SQL
    "select","insert","update","delete","from","where","join","inner","left",
    "right","full","on","group","by","order","having","limit","offset","union",
    "distinct","as","into","values","set","create","table","view","index",
    "drop","alter","add","column","primary","key","foreign","references",
    "constraint","check","default","null","not","and","or","in","exists",

    // TOML/YAML/INI
    "true","false","null","yes","no","on","off",

    // Misc
    "print","echo","input","read","write","open","close","main","args","argc","argv",
};

static const char OPS_CODE[] =
    "{}()[];,+-*/%<>&|^!=~?:.@$`\\#";

static const char STR_PUNCT[] =
    "{}[]:,\"'=<>/;()!@#$%^&*|~`\\.";

static const char *KW_STRUCT[] = {
    // JSON/TOML/YAML/INI booleans/null
    "true","false","null","yes","no","on","off",

    // Additional structured keywords
    "none","nil","empty","undefined","default","required","optional",
    "enabled","disabled","active","inactive","success","error","fail","warning",
    "info","debug","trace","fatal","critical","unknown","pending","complete",
    "open","closed","locked","unlocked","visible","hidden","readonly","writeonly",
    "public","private","protected","internal","external","global","local",
    "user","admin","guest","system","root","superuser","owner","group",
    "config","settings","options","parameters","values","items","list","array",
    "object","map","dict","table","section","entry","field","row","column",
    "start","end","first","last","next","prev","previous","current","index",
    "count","size","length","capacity","limit","max","min","average","median",
    "sum","total","amount","rate","percent","ratio","score","grade",
    "date","time","timestamp","created","modified","updated","deleted",
    "path","file","dir","folder","name","type","id","uuid","guid","key",
    "value","data","info","desc","description","title","label","tag","note",
    "status","state","mode","level","priority","rank","order","sort",
    "input","output","result","response","request","message","event","action",
    "source","target","destination","origin","host","port","address","ip",
    "url","uri","link","ref","reference","hash","checksum","signature",
    "version","release","build","commit","branch","tag","merge","pull","push",
    "login","logout","register","signup","signin","signout","auth","authorize",
    "token","session","cookie","cache","store","load","save","read","write",
    "connect","disconnect","send","receive","upload","download","import","export",
    "start","stop","pause","resume","run","execute","launch","init","initialize",
    "shutdown","restart","reset","clear","flush","sync","update","upgrade",
    "install","uninstall","remove","delete","add","create","edit","change",
    "set","get","find","search","replace","filter","sort","group","aggregate",
    "split","join","combine","merge","diff","patch","apply","revert","rollback",
    "backup","restore","copy","move","rename","duplicate","clone","fork",
    "lock","unlock","open","close","enable","disable","show","hide","display",
    "print","echo","log","trace","debug","warn","error","fail","success",
    "ok","cancel","confirm","accept","reject","approve","deny","allow","block",
    "grant","revoke","subscribe","unsubscribe","follow","unfollow","like","dislike",
    "vote","rate","review","comment","reply","share","post","publish","draft",
    "archive","unarchive","favorite","bookmark","star","flag","report",
    "invite","join","leave","kick","ban","mute","unmute","promote","demote",
    "add","remove","update","edit","delete","create","destroy","build","compile",
    "test","run","execute","deploy","release","publish","install","uninstall",
    "upgrade","downgrade","rollback","restore","backup","sync","merge","rebase",
    "commit","push","pull","fetch","clone","fork","branch","tag","checkout",
    "init","status","log","diff","reset","clean","stash","pop","apply",
    "cherry-pick","revert","blame","bisect","submodule","remote","origin",
    "upstream","downstream","mirror","snapshot","archive","bundle","patch",
    "issue","bug","feature","task","todo","note","comment","review","approve",
    "request","response","message","event","action","trigger","handler",
    "listener","observer","subscriber","publisher","producer","consumer",
    "queue","stack","heap","tree","graph","list","array","map","set","dict",
    "table","row","column","cell","field","record","entry","item","element",
    "node","edge","vertex","link","connection","relation","reference",
    "parent","child","sibling","ancestor","descendant","root","leaf","branch",
    "path","route","track","trace","trail","way","line","curve","circle",
    "point","dot","pixel","block","chunk","segment","slice","piece","part",
    "section","zone","area","region","domain","range","interval","window",
    "frame","page","screen","view","panel","tab","group","box","container",
    "form","input","output","button","label","field","checkbox","radio",
    "select","option","dropdown","listbox","slider","spinner","switch",
    "toggle","menu","item","link","anchor","image","icon","picture","photo",
    "video","audio","media","file","document","text","html","xml","json",
    "yaml","toml","ini","csv","tsv","md","markdown","rst","adoc","pdf",
    "doc","docx","xls","xlsx","ppt","pptx","odt","ods","odp","rtf","tex",
    "latex","bib","log","conf","cfg","config","settings","properties",
    "env","environment","profile","template","sample","example","demo",
    "test","case","suite","spec","scenario","step","assert","expect",
    "verify","check","validate","compare","match","diff","patch","fix",
    "resolve","close","open","assign","unassign","label","tag","milestone",
    "release","version","build","commit","branch","merge","pull","push",
    "fork","clone","init","status","log","diff","reset","clean","stash",
    "pop","apply","cherry-pick","revert","blame","bisect","submodule",
    "remote","origin","upstream","downstream","mirror","snapshot","archive",
    "bundle","patch","issue","bug","feature","task","todo","note","comment",
    "review","approve","request","response","message","event","action",
    "trigger","handler","listener","observer","subscriber","publisher",
    "producer","consumer","queue","stack","heap","tree","graph","list",
    "array","map","set","dict","table","row","column","cell","field",
    "record","entry","item","element","node","edge","vertex","link",
    "connection","relation","reference","parent","child","sibling",
    "ancestor","descendant","root","leaf","branch","path","route",
    "track","trace","trail","way","line","curve","circle","point",
    "dot","pixel","block","chunk","segment","slice","piece","part",
    "section","zone","area","region","domain","range","interval",
    "window","frame","page","screen","view","panel","tab","group",
    "box","container","form","input","output","button","label",
    "field","checkbox","radio","select","option","dropdown",
    "listbox","slider","spinner","switch","toggle","menu","item",
    "link","anchor","image","icon","picture","photo","video",
    "audio","media","file","document","text","html","xml","json",
    "yaml","toml","ini","csv","tsv","md","markdown","rst","adoc",
    "pdf","doc","docx","xls","xlsx","ppt","pptx","odt","ods",
    "odp","rtf","tex","latex","bib","log","conf","cfg","config",
    "settings","properties","env","environment","profile","template",
    "sample","example","demo","test","case","suite","spec","scenario",
    "step","assert","expect","verify","check","validate","compare",
    "match","diff","patch","fix","resolve","close","open","assign",
    "unassign","label","tag","milestone","release","version","build",
    "commit","branch","merge","pull","push","fork","clone","init",
    "status","log","diff","reset","clean","stash","pop","apply",
    "cherry-pick","revert","blame","bisect","submodule","remote",
    "origin","upstream","downstream","mirror","snapshot","archive",
    "bundle","patch"
};

#define is_number_char(c) (isdigit((unsigned char)(c)) || (c)=='-' || (c)=='.')

/* ---------------------------------------------------------------------
    Helper: check keyword list (linear scan). Could be optimized later.
    --------------------------------------------------------------------- */
static int is_in_table(const char *tok, const char *table[], size_t n) {
     for (size_t i = 0; i < n; ++i) {
          if (table[i] == NULL) continue;
          if (strcmp(tok, table[i]) == 0) return 1;
     }
     return 0;
}

/* ---------------------------------------------------------------------
    Helper: apply color markup tags in output buffer.
    --------------------------------------------------------------------- */
static void apply_color_tags(const char *src, char *dst, size_t dst_size) {
    size_t si = 0, di = 0;
    while (src[si] != '\0' && di < dst_size - 1) {
        if (src[si] == '{') {
            size_t tag_start = si + 1;
            size_t tag_end = tag_start;
            while (src[tag_end] != '\0' && src[tag_end] != '}') tag_end++;
            if (src[tag_end] == '}') {
                char tag[64];
                size_t tag_len = tag_end - tag_start;
                if (tag_len < sizeof(tag)) {
                    memcpy(tag, src + tag_start, tag_len);
                    tag[tag_len] = '\0';
                    // Map tag to ANSI escape sequence
                    if (strcmp(tag, "red") == 0)
                        di += snprintf(dst + di, dst_size - di, "\033[31m");
                    else if (strcmp(tag, "green") == 0)
                        di += snprintf(dst + di, dst_size - di, "\033[32m");
                    else if (strcmp(tag, "yellow") == 0)
                        di += snprintf(dst + di, dst_size - di, "\033[33m");
                    else if (strcmp(tag, "blue") == 0)
                        di += snprintf(dst + di, dst_size - di, "\033[34m");
                    else if (strcmp(tag, "magenta") == 0)
                        di += snprintf(dst + di, dst_size - di, "\033[35m");
                    else if (strcmp(tag, "cyan") == 0)
                        di += snprintf(dst + di, dst_size - di, "\033[36m");
                    else if (strcmp(tag, "normal") == 0)
                        di += snprintf(dst + di, dst_size - di, "\033[0m");
                    else if (strcmp(tag, "lightblue") == 0)
                        di += snprintf(dst + di, dst_size - di, "\033[94m");
                    else if (strstr(tag, "bold"))
                        di += snprintf(dst + di, dst_size - di, "\033[1m");
                    else if (strstr(tag, "underline"))
                        di += snprintf(dst + di, dst_size - di, "\033[4m");
                    else if (strstr(tag, "dim"))
                        di += snprintf(dst + di, dst_size - di, "\033[2m");
                    else if (strstr(tag, "reversed"))
                        di += snprintf(dst + di, dst_size - di, "\033[7m");
                    else if (strncmp(tag, "pos:", 4) == 0) {
                        // Example: {pos:top} moves cursor to top
                        if (strcmp(tag + 4, "top") == 0)
                            di += snprintf(dst + di, dst_size - di, "\033[H");
                        else if (strcmp(tag + 4, "bottom") == 0)
                            di += snprintf(dst + di, dst_size - di, "\033[999B");
                        else if (strcmp(tag + 4, "left") == 0)
                            di += snprintf(dst + di, dst_size - di, "\033[1G");
                        else if (strcmp(tag + 4, "right") == 0)
                            di += snprintf(dst + di, dst_size - di, "\033[999C");
                    }
                    // Add more tags as needed
                }
                si = tag_end + 1;
                continue;
            }
        }
        dst[di++] = src[si++];
    }
    dst[di] = '\0';
}

/* ---------------------------------------------------------------------
    The main function (integrated)
    --------------------------------------------------------------------- */
int fossil_shark_view(ccstring path, bool number_lines,
                        bool number_non_blank, bool squeeze_blank,
                        int head_lines, int tail_lines, bool show_time) {

    if (path == cnull) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red}Error:{normal} File path must be specified.\n");
        return 1;
    }

    /* Determine file type by extension */
    bool is_media = false, is_code = false, is_structured = false;
    const char *ext = strrchr(path, '.');
    if (ext) {
        if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0 ||
             strcmp(ext, ".png") == 0 || strcmp(ext, ".gif") == 0 ||
             strcmp(ext, ".mp3") == 0 || strcmp(ext, ".mp4") == 0 ||
             strcmp(ext, ".wav") == 0 || strcmp(ext, ".avi") == 0 ||
             strcmp(ext, ".mov") == 0 || strcmp(ext, ".flac") == 0) {
             is_media = true;
        } else if (strcmp(ext, ".json") == 0 || strcmp(ext, ".xml") == 0 ||
                    strcmp(ext, ".yaml") == 0 || strcmp(ext, ".yml") == 0 ||
                    strcmp(ext, ".toml") == 0 || strcmp(ext, ".ini") == 0 ||
                    strcmp(ext, ".csv") == 0) {
             is_structured = true;
        } else if (strcmp(ext, ".c") == 0 || strcmp(ext, ".cpp") == 0 ||
                    strcmp(ext, ".h") == 0 || strcmp(ext, ".hpp") == 0 ||
                    strcmp(ext, ".py") == 0 || strcmp(ext, ".js") == 0 ||
                    strcmp(ext, ".java") == 0 || strcmp(ext, ".cs") == 0 ||
                    strcmp(ext, ".go") == 0 || strcmp(ext, ".rs") == 0 ||
                    strcmp(ext, ".sh") == 0 || strcmp(ext, ".html") == 0 ||
                    strcmp(ext, ".css") == 0) {
             is_code = true;
        }
    }

    /* Open file */
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, path, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red}Error:{normal} ");
        perror(path);
        return 1;
    }

    /* Optional timestamp info */
    if (show_time) {
#ifdef _WIN32
        struct _stat st;
        if (_stat(path, &st) == 0) {
             fossil_io_printf("{blue}File:{normal} %s\n", path);
             fossil_io_printf("{blue}Size:{normal} %ld bytes\n", (long)st.st_size);
             char time_buf[64];
             struct tm *tm_info = localtime(&st.st_mtime);
             if (tm_info) {
                 strftime(time_buf, sizeof(time_buf), "%c", tm_info);
                 fossil_io_printf("{blue}Modified:{normal} %s\n", time_buf);
             }
        }
#else
        struct stat st;
        if (stat(path, &st) == 0) {
             fossil_io_printf("{blue}File:{normal} %s\n", path);
             fossil_io_printf("{blue}Size:{normal} %ld bytes\n", (long)st.st_size);
             fossil_io_printf("{blue}Modified:{normal} %s", ctime(&st.st_mtime));
        }
#endif
    }

    /* Tail buffering support */
    cstring *lines = cnull;
    size_t count = 0, capacity = 0;
    char buffer[8192];

    while (fossil_io_gets_from_stream(buffer, sizeof(buffer), &stream)) {

        /* Trim right newline but preserve leading/trailing spaces (we visualize them) */
        fossil_io_trim(buffer);

        if (squeeze_blank && strlen(buffer) == 0) {
             if (count > 0 && cnotnull(lines) && strlen(lines[count - 1]) == 0) continue;
        }

        /* Enhanced token-aware colorization */
        char colored_buf[8192 * 4];
        size_t j = 0;

        int in_block_comment = 0;

        for (size_t i = 0; buffer[i] != '\0' && j < sizeof(colored_buf) - 128; ++i) {
             unsigned char c = (unsigned char)buffer[i];

             /* -------------------------
                (A) Visual whitespace
                ------------------------- */
             if (c == ' ') {
                 j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                "{normal}{dim}·{normal}");
                 continue;
             }
             if (c == '\t') {
                 j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                "{cyan}{dim}→{normal}");
                 continue;
             }

             /* -------------------------
                (B) Media files: magenta everything
                ------------------------- */
             if (is_media) {
                 j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                "{magenta}%c{normal}", c);
                 continue;
             }

             /* -------------------------
                (C) Structured formats: JSON/YAML/TOML/INI/CSV
                - punctuation -> cyan
                - boolean/null -> green,bold
                - numbers -> yellow,bold
                - default -> normal
                ------------------------- */
             if (is_structured) {
                 if (strchr(STR_PUNCT, c)) {
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                    "{cyan}%c{normal}", c);
                     continue;
                 }

                 /* Simple token scans for keywords (true/false/null/etc.) */
                 if (isalpha(c)) {
                     char tok[128];
                     size_t t = 0;
                     while ((isalnum((unsigned char)buffer[i]) || buffer[i] == '_') && t < sizeof(tok)-1) {
                          tok[t++] = buffer[i];
                          i++;
                     }
                     tok[t] = '\0';
                     i--; /* rewind one char as main loop will advance */

                     if (is_in_table(tok, KW_STRUCT, sizeof(KW_STRUCT)/sizeof(KW_STRUCT[0]))) {
                          j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                        "{green,bold}%s{normal}", tok);
                     } else {
                          j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                        "{normal}%s", tok);
                     }
                     continue;
                 }

                 /* Numbers */
                 if (is_number_char(c)) {
                     size_t k = i;
                     char numtok[64];
                     size_t nk = 0;
                     while (buffer[k] != '\0' && is_number_char(buffer[k]) && nk < sizeof(numtok)-1) {
                          numtok[nk++] = buffer[k++];
                     }
                     numtok[nk] = '\0';
                     /* advance main index */
                     i = k - 1;
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                    "{yellow,bold}%s{normal}", numtok);
                     continue;
                 }

                 /* fallback */
                 j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                "{normal}%c", c);
                 continue;
             }

             /* -------------------------
                (D) Code files: keywords, operators, comments, strings, numbers, tags
                ------------------------- */
             if (is_code) {

                 /* (0) Color tag markup: {red}, {green,bold}, {pos:top}, etc. */
                 if (c == '{') {
                     size_t k = i;
                     while (buffer[k] != '\0' && buffer[k] != '}') k++;
                     if (buffer[k] == '}') {
                          size_t taglen = k - i + 1;
                          char tag[64];
                          if (taglen < sizeof(tag)) {
                              memcpy(tag, buffer + i, taglen);
                              tag[taglen] = '\0';
                              j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                             "%s", tag); // keep tag for later processing
                              i = k;
                              continue;
                          }
                     }
                 }

                 /* (1) Comments: // and # - rest of line; ... handled too */
                 if (!in_block_comment && c == '/' && buffer[i+1] == '/') {
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                    "{lightblue}//");
                     i += 2;
                     /* Colorize the rest of the line as comment, no keywords */
                     while (buffer[i] != '\0' && j < sizeof(colored_buf) - 1) {
                         j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                       "%c", buffer[i]);
                         i++;
                     }
                     break;
                 }
                 if (!in_block_comment && c == '#') {
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                    "{lightblue}#");
                     i++;
                     while (buffer[i] != '\0' && j < sizeof(colored_buf) - 1) {
                         j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                       "%c", buffer[i]);
                         i++;
                     }
                     break;
                 }
                 /* (1.1) Doxygen/Javadoc style comment: ... */
                 if (!in_block_comment && c == '/' && buffer[i+1] == '*' && buffer[i+2] == '*') {
                     size_t k = i + 3;
                     in_block_comment = 1;
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                   "{lightblue,bold}/**");
                     while (buffer[k] != '\0') {
                          if (buffer[k] == '*' && buffer[k+1] == '/') {
                              j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                            "%c%c", buffer[k], buffer[k+1]);
                              k += 2;
                              in_block_comment = 0;
                              break;
                          }
                          j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                        "%c", buffer[k]);
                          k++;
                     }
                     i = k - 1;
                     continue;
                 }
                 /* (1.2) C-style block comment: ... */
                 if (!in_block_comment && c == '/' && buffer[i+1] == '*') {
                     size_t k = i + 2;
                     in_block_comment = 1;
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                   "{lightblue}/*");
                     while (buffer[k] != '\0') {
                          if (buffer[k] == '*' && buffer[k+1] == '/') {
                              j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                            "%c%c", buffer[k], buffer[k+1]);
                              k += 2;
                              in_block_comment = 0;
                              break;
                          }
                          j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                        "%c", buffer[k]);
                          k++;
                     }
                     i = k - 1;
                     continue;
                 }
                 /* (1.3) Inside block comment: color everything light blue */
                 if (in_block_comment) {
                     while (buffer[i] != '\0' && j < sizeof(colored_buf) - 1) {
                         if (buffer[i] == '*' && buffer[i+1] == '/') {
                             j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                           "{lightblue}*/{normal}");
                             i += 1;
                             in_block_comment = 0;
                             break;
                         }
                         j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                       "{lightblue}%c{normal}", buffer[i]);
                         i++;
                     }
                     continue;
                 }

                 /* (2) Strings: "..." or '...' - punctuation cyan, contents magenta */
                 if (c == '"' || c == '\'') {
                     char q = c;
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                    "{cyan}%c{normal}", q);
                     size_t k = i + 1;
                     while (buffer[k] != '\0') {
                          if (buffer[k] == '\\' && buffer[k+1] != '\0') {
                              char esc[3] = { buffer[k], buffer[k+1], '\0' };
                              j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                             "{magenta}%s{normal}", esc);
                              k += 2;
                              continue;
                          }
                          if (buffer[k] == q) {
                              j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                             "{cyan}%c{normal}", q);
                              break;
                          }
                          size_t run_start = k;
                          while (buffer[k] != '\0' && buffer[k] != q && buffer[k] != '\\') k++;
                          size_t run_len = k - run_start;
                          if (run_len > 0) {
                              char *run = (char *)fossil_sys_memory_alloc(run_len + 1);
                              if (run) {
                                  memcpy(run, buffer + run_start, run_len);
                                  run[run_len] = '\0';
                                  j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                                 "{magenta}%s{normal}", run);
                                  fossil_sys_memory_free(run);
                              } else {
                                  for (size_t z = run_start; z < run_start + run_len; ++z) {
                                       j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                                     "{magenta}%c{normal}", buffer[z]);
                                  }
                              }
                          }
                     }
                     while (buffer[i+1] != '\0' && buffer[i+1] != q) i++;
                     if (buffer[i+1] == q) i++;
                     continue;
                 }

                 /* (3) Operators & punctuation */
                 if (strchr(OPS_CODE, c)) {
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                    "{yellow}%c{normal}", c);
                     continue;
                 }
                 if (strchr(STR_PUNCT, c)) {
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                    "{cyan}%c{normal}", c);
                     continue;
                 }

                 /* (4) Numbers: digit or -/.\ leading */
                 if (is_number_char(c)) {
                     size_t k = i;
                     char numtok[128];
                     size_t nk = 0;
                     while (buffer[k] != '\0' && (isdigit((unsigned char)buffer[k]) || buffer[k] == '.' || buffer[k] == '-' || buffer[k] == '+' || buffer[k] == 'e' || buffer[k] == 'E') && nk < sizeof(numtok)-1) {
                          numtok[nk++] = buffer[k++];
                     }
                     numtok[nk] = '\0';
                     i = k - 1;
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                    "{yellow,bold}%s{normal}", numtok);
                     continue;
                 }

                 /* (5) Identifiers / keywords */
                 if (isalpha(c) || c == '_') {
                     char tok[256];
                     size_t t = 0;
                     size_t k = i;
                     while ((isalnum((unsigned char)buffer[k]) || buffer[k] == '_') && t < sizeof(tok)-1) {
                          tok[t++] = buffer[k++];
                     }
                     tok[t] = '\0';
                     i = k - 1;

                     if (is_in_table(tok, KW_CODE, sizeof(KW_CODE)/sizeof(KW_CODE[0]))) {
                          j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                        "{blue,bold}%s{normal}", tok);
                          continue;
                     }
                     if (is_in_table(tok, KW_STRUCT, sizeof(KW_STRUCT)/sizeof(KW_STRUCT[0]))) {
                          j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                        "{green,bold}%s{normal}", tok);
                          continue;
                     }
                     j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                    "{normal}%s", tok);
                     continue;
                 }

                 /* fallback single char */
                 j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                "{normal}%c", c);
                 continue;
             } /* end is_code */

             /* -------------------------
                (E) Default: color green
                ------------------------- */
             j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                            "{green}%c{normal}", c);
        } /* end for */

        colored_buf[j] = '\0';

        /* Apply color tags to output buffer */
        char ansi_buf[sizeof(colored_buf) * 2];
        apply_color_tags(colored_buf, ansi_buf, sizeof(ansi_buf));

        /* Output or buffer for tail-lines */
        if (tail_lines > 0) {
             if (count >= capacity) {
                 capacity = capacity ? capacity * 2 : 128;
                 cstring *new_lines;
                 if (lines == cnull) {
                     new_lines = (cstring *)fossil_sys_memory_alloc(capacity * sizeof(cstring));
                 } else {
                     new_lines = (cstring *)fossil_sys_memory_realloc(lines, capacity * sizeof(cstring));
                 }
                 if (new_lines == cnull) cpanic("Memory allocation failed for lines buffer");
                 lines = new_lines;
             }
             lines[count++] = fossil_io_cstring_dup_safe(ansi_buf, strlen(ansi_buf));
        } else {
             if (head_lines == 0 || (int)count < head_lines) {
                 if (number_lines)
                     fossil_io_printf("{blue}%6zu{normal}\t%s\n", count + 1, ansi_buf);
                 else if (number_non_blank && strlen(buffer) > 0)
                     fossil_io_printf("{blue}%6zu{normal}\t%s\n", count + 1, ansi_buf);
                 else
                     fossil_io_printf("%s\n", ansi_buf);
             }
             count++;
             if (head_lines > 0 && (int)count >= head_lines) break;
        }
    } /* end while */

    fossil_fstream_close(&stream);

    /* Tail output flush */
    if (tail_lines > 0 && count > 0 && cnotnull(lines)) {
        size_t start = count > (size_t)tail_lines ? count - tail_lines : 0;
        for (size_t i = start; i < count; i++) {
             if (number_lines)
                 fossil_io_printf("{blue}%6zu{normal}\t%s\n", i + 1, lines[i]);
             else if (number_non_blank && strlen(lines[i]) > 0)
                 fossil_io_printf("{blue}%6zu{normal}\t%s\n", i + 1, lines[i]);
             else
                 fossil_io_printf("%s\n", lines[i]);
             fossil_io_cstring_free_safe(&lines[i]);
        }
        fossil_sys_memory_free(lines);
        cnullify(lines);
    }

    return 0;
}
