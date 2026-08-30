// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef CONTENTTYPE_H
#define CONTENTTYPE_H

#endif 
enum class CONTENTTYPE_H ContentType
{
	//application options
	application_octet_stream,
	application_x_abiword,
	application_x_freearc,
	application_x_bzip,
	application_x_bzip2,
	application_ogg,

	//Audio Types
	audio_aac,
	audio_mpeg,
	audio_ogg,
	audio_wav,
	audio_webm,

	//Video Types
	video_x_msvideo,
	video_mpeg,
	video_ogg,
	video_mp2t,
	video_webm,

	//zip types
	application_epub_zip,
	application_gzip,
	application_vnd_rar,
	application_x_tar,
	application_zip,
	application_x_7z_compressed,

	//image
	image_bmp,
	image_gif,
	image_jpeg,
	image_png,
	image_svg_xml,
	image_tiff,
	image_webp,

	//format types
	application_x_csh,
	application_msword,
	application_vnd_openxmlformats_officedocument_wordprocessingml_document,
	text_html,
	text_javascript,
	application_json,
	application_ld_json,
	application_vnd_ms_powerpoint,
	application_vnd_openxmlformats_officedocument_presentationml_presentation,
	application_xhtml_xml,
	application_vnd_openxmlformats_officedocument_spreadsheetml_sheet,

	//doc
	text_csv,
	audio_basic,
	application_pdf

};


#ifndef CONTENTTYPERESOLVER_H
#define CONTENTTYPERESOLVER_H
//#include "pch.h";
#endif 

class CONTENTTYPERESOLVER_H ContentTypeResolver {

public:
	static string ResolveExtensionIRI(ContentType option);
};