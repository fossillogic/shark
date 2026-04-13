/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/code/cryptic.h"

/**
 * Encode or decode text using various ciphers
 * @param text Text to encode or decode
 * @param encode Encode the text
 * @param decode Decode the text
 * @param cipher Cipher type to use ("caesar", "vigenere", "base64", "base32", "binary", "morse", "baconian", "railfence", "haxor", "leet", "rot13", "atbash")
 * @return 0 on success, non-zero on error
 */
int fossil_shark_cryptic(const char *text, bool encode, bool decode, const char *cipher)
{
    if (!text || !cipher)
    {
        return 1; // Error: invalid arguments
    }

    if (!encode && !decode)
    {
        return 1; // Error: neither encode nor decode specified
    }

    if (encode && decode)
    {
        return 1; // Error: both encode and decode specified
    }

    char *result = NULL;

    if (encode)
    {
        result = fossil_io_cipher_encode(text, cipher);
    }
    else
    {
        result = fossil_io_cipher_decode(text, cipher);
    }

    if (!result)
    {
        return 1; // Error: cipher operation failed
    }

    // Output the result
    fossil_io_printf("%s\n", result);
    free(result);
    return 0; // Success
}
