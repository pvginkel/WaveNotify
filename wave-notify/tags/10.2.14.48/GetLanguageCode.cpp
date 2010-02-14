/*
 * This file is part of Google Wave Notifier.
 *
 * Google Wave Notifier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Google Wave Notifier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Google Wave Notifier.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "include.h"

typedef struct
{
	WORD nLanguageID;
	LPCWSTR szCode;
} LOCALE_CODE, * LPLOCALE_CODE;

static LOCALE_CODE g_lpLocales[] = {
	{ 0x0436, L"af-ZA" }, { 0x041c, L"sq-AL" }, { 0x0484, L"gsw-FR" }, { 0x045e, L"am-ET" },
	{ 0x1401, L"ar-DZ" }, { 0x3c01, L"ar-BH" }, { 0x0c01, L"ar-EG" }, { 0x0801, L"ar-IQ" },
	{ 0x2c01, L"ar-JO" }, { 0x3401, L"ar-KW" }, { 0x3001, L"ar-LB" }, { 0x1001, L"ar-LY" },
	{ 0x1801, L"ar-MA" }, { 0x2001, L"ar-OM" }, { 0x4001, L"ar-QA" }, { 0x0401, L"ar-SA" },
	{ 0x2801, L"ar-SY" }, { 0x1c01, L"ar-TN" }, { 0x3801, L"ar-AE" }, { 0x2401, L"ar-YE" },
	{ 0x042b, L"hy-AM" }, { 0x044d, L"as-IN" }, { 0x082c, L"az-Cyrl-AZ" }, { 0x042c, L"az-Latn-AZ" },
	{ 0x046d, L"ba-RU" }, { 0x042d, L"eu-ES" }, { 0x0423, L"be-BY" }, { 0x0445, L"bn-IN" },
	{ 0x201a, L"bs-Cyrl-BA" }, { 0x141a, L"bs-Latn-BA" }, { 0x047e, L"br-FR" }, { 0x0402, L"bg-BG" },
	{ 0x0403, L"ca-ES" }, { 0x0c04, L"zh-HK" }, { 0x1404, L"zh-MO" }, { 0x0804, L"zh-CN" },
	{ 0x1004, L"zh-SG" }, { 0x0404, L"zh-TW" }, { 0x101a, L"hr-BA" }, { 0x041a, L"hr-HR" },
	{ 0x0405, L"cs-CZ" }, { 0x0406, L"da-DK" }, { 0x048c, L"gbz-AF" }, { 0x0465, L"dv-MV" },
	{ 0x0813, L"nl-BE" }, { 0x0413, L"nl-NL" }, { 0x0c09, L"en-AU" }, { 0x2809, L"en-BZ" },
	{ 0x1009, L"en-CA" }, { 0x2409, L"en-029" }, { 0x4009, L"en-IN" }, { 0x1809, L"en-IE" },
	{ 0x2009, L"en-JM" }, { 0x4409, L"en-MY" }, { 0x1409, L"en-NZ" }, { 0x3409, L"en-PH" },
	{ 0x4809, L"en-SG" }, { 0x1c09, L"en-ZA" }, { 0x2c09, L"en-TT" }, { 0x0809, L"en-GB" },
	{ 0x0409, L"en-US" }, { 0x3009, L"en-ZW" }, { 0x0425, L"et-EE" }, { 0x0438, L"fo-FO" },
	{ 0x0464, L"fil-PH" }, { 0x040b, L"fi-FI" }, { 0x080c, L"fr-BE" }, { 0x0c0c, L"fr-CA" },
	{ 0x040c, L"fr-FR" }, { 0x140c, L"fr-LU" }, { 0x180c, L"fr-MC" }, { 0x100c, L"fr-CH" },
	{ 0x0462, L"fy-NL" }, { 0x0456, L"gl-ES" }, { 0x0437, L"ka-GE" }, { 0x0c07, L"de-AT" },
	{ 0x0407, L"de-DE" }, { 0x1407, L"de-LI" }, { 0x1007, L"de-LU" }, { 0x0807, L"de-CH" },
	{ 0x0408, L"el-GR" }, { 0x046f, L"kl-GL" }, { 0x0447, L"gu-IN" }, { 0x0468, L"ha-Latn-NG" },
	{ 0x040d, L"he-IL" }, { 0x0439, L"hi-IN" }, { 0x040e, L"hu-HU" }, { 0x040f, L"is-IS" },
	{ 0x0470, L"ig-NG" }, { 0x0421, L"id-ID" }, { 0x085d, L"iu-Latn-CA" }, { 0x045d, L"iu-Cans-CA" },
	{ 0x083c, L"ga-IE" }, { 0x0410, L"it-IT" }, { 0x0810, L"it-CH" }, { 0x0411, L"ja-JP" },
	{ 0x044b, L"kn-IN" }, { 0x043f, L"kk-KZ" }, { 0x0453, L"kh-KH" }, { 0x0486, L"qut-GT" },
	{ 0x0487, L"rw-RW" }, { 0x0457, L"kok-IN" }, { 0x0412, L"ko-KR" }, { 0x0440, L"ky-KG" },
	{ 0x0454, L"lo-LA" }, { 0x0426, L"lv-LV" }, { 0x0427, L"lt-LT" }, { 0x082e, L"dsb-DE" },
	{ 0x046e, L"lb-LU" }, { 0x042f, L"mk-MK" }, { 0x083e, L"ms-BN" }, { 0x043e, L"ms-MY" },
	{ 0x044c, L"ml-IN" }, { 0x043a, L"mt-MT" }, { 0x0481, L"mi-NZ" }, { 0x047a, L"arn-CL" },
	{ 0x044e, L"mr-IN" }, { 0x047c, L"moh-CA" }, { 0x0450, L"mn-Cyrl-MN" }, { 0x0850, L"mn-Mong-CN" },
	{ 0x0461, L"ne-NP" }, { 0x0414, L"nb-NO" }, { 0x0814, L"nn-NO" }, { 0x0482, L"oc-FR" },
	{ 0x0448, L"or-IN" }, { 0x0463, L"ps-AF" }, { 0x0429, L"fa-IR" }, { 0x0415, L"pl-PL" },
	{ 0x0416, L"pt-BR" }, { 0x0816, L"pt-PT" }, { 0x0446, L"pa-IN" }, { 0x046b, L"quz-BO" },
	{ 0x086b, L"quz-EC" }, { 0x0c6b, L"quz-PE" }, { 0x0418, L"ro-RO" }, { 0x0417, L"rm-CH" },
	{ 0x0419, L"ru-RU" }, { 0x243b, L"smn-FI" }, { 0x103b, L"smj-NO" }, { 0x143b, L"smj-SE" },
	{ 0x0c3b, L"se-FI" }, { 0x043b, L"se-NO" }, { 0x083b, L"se-SE" }, { 0x203b, L"sms-FI" },
	{ 0x183b, L"sma-NO" }, { 0x1c3b, L"sma-SE" }, { 0x044f, L"sa-IN" }, { 0x1c1a, L"sr-Cyrl-BA" },
	{ 0x181a, L"sr-Latn-BA" }, { 0x0c1a, L"sr-Cyrl-CS" }, { 0x081a, L"sr-Latn-CS" }, { 0x046c, L"ns-ZA" },
	{ 0x0432, L"tn-ZA" }, { 0x045b, L"si-LK" }, { 0x041b, L"sk-SK" }, { 0x0424, L"sl-SI" },
	{ 0x2c0a, L"es-AR" }, { 0x400a, L"es-BO" }, { 0x340a, L"es-CL" }, { 0x240a, L"es-CO" },
	{ 0x140a, L"es-CR" }, { 0x1c0a, L"es-DO" }, { 0x300a, L"es-EC" }, { 0x440a, L"es-SV" },
	{ 0x100a, L"es-GT" }, { 0x480a, L"es-HN" }, { 0x080a, L"es-MX" }, { 0x4c0a, L"es-NI" },
	{ 0x180a, L"es-PA" }, { 0x3c0a, L"es-PY" }, { 0x280a, L"es-PE" }, { 0x500a, L"es-PR" },
	{ 0x0c0a, L"es-ES" }, { 0x040a, L"es-ES_tradnl" }, { 0x540a, L"es-US" }, { 0x380a, L"es-UY" },
	{ 0x200a, L"es-VE" }, { 0x0441, L"sw-KE" }, { 0x081d, L"sv-FI" }, { 0x041d, L"sv-SE" },
	{ 0x045a, L"syr-SY" }, { 0x0428, L"tg-Cyrl-TJ" }, { 0x085f, L"tmz-Latn-DZ" }, { 0x0449, L"ta-IN" },
	{ 0x0444, L"tt-RU" }, { 0x044a, L"te-IN" }, { 0x041e, L"th-TH" }, { 0x0851, L"bo-BT" },
	{ 0x0451, L"bo-CN" }, { 0x041f, L"tr-TR" }, { 0x0442, L"tk-TM" }, { 0x0480, L"ug-CN" },
	{ 0x0422, L"uk-UA" }, { 0x042e, L"wen-DE" }, { 0x0820, L"tr-IN" }, { 0x0420, L"ur-PK" },
	{ 0x0843, L"uz-Cyrl-UZ" }, { 0x0443, L"uz-Latn-UZ" }, { 0x042a, L"vi-VN" }, { 0x0452, L"cy-GB" },
	{ 0x0488, L"wo-SN" }, { 0x0434, L"xh-ZA" }, { 0x0485, L"sah-RU" }, { 0x0478, L"ii-CN" },
	{ 0x046a, L"yo-NG" }, { 0x0435, L"zu-ZA" },
	{ 0x0, NULL }
};

LPCWSTR GetLanguageCode()
{
	static BOOL fInitialised = FALSE;
	static LPCWSTR szResult = NULL;

	if (!fInitialised)
	{
		fInitialised = TRUE;

		WORD nLanguageID = LANGIDFROMLCID(GetUserDefaultLCID());

		LPLOCALE_CODE lpLocaleCode = g_lpLocales;

		szResult = NULL;

		while (lpLocaleCode->nLanguageID > 0)
		{
			if (lpLocaleCode->nLanguageID == nLanguageID)
			{
				szResult = lpLocaleCode->szCode;
				break;
			}

			lpLocaleCode++;
		}
	}

	return szResult;
}
