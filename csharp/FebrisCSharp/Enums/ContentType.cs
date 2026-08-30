// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Enums
{
    public enum ContentType
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
        
    }
    class ContentTypeResolver
    {
        internal static string ResolveExtensionIRI(ContentType option)
        {
            switch (option)
            {
                case ContentType.application_octet_stream:
                    return "application/octet-stream";
                case ContentType.application_x_abiword:
                    return "application/x-abiword";
                case ContentType.application_x_freearc:
                    return "application/x-freearc";
                case ContentType.application_x_bzip:
                    return "application/x-bzip";
                case ContentType.application_x_bzip2:
                    return "application/x-bzip2";
                case ContentType.application_ogg:
                    return "application/ogg";
                case ContentType.audio_aac:
                    return "audio/aac";
                case ContentType.audio_mpeg:
                    return "audio/mpeg";
                case ContentType.audio_ogg:
                    return "audio/ogg";
                case ContentType.audio_wav:
                    return "audio/wav";
                case ContentType.audio_webm:
                    return "audio/webm";
                case ContentType.video_x_msvideo:
                    return "video/x-msvideo";
                case ContentType.video_mpeg:
                    return "video/mpeg";
                case ContentType.video_ogg:
                    return "video/ogg";
                case ContentType.video_mp2t:
                    return "video/mp2t";
                case ContentType.video_webm:
                    return "video/webm";
                case ContentType.application_epub_zip:
                    return "application/epub+zip";
                case ContentType.application_gzip:
                    return "application/gzip";
                case ContentType.application_vnd_rar:
                    return "application/vnd.rar";
                case ContentType.application_x_tar:
                    return "application/x-tar";
                case ContentType.application_zip:
                    return "application/zip";
                case ContentType.application_x_7z_compressed:
                    return "application/x-7z-compressed";
                case ContentType.image_bmp:
                    return "image/bmp";
                case ContentType.image_gif:
                    return "image/gif";
                case ContentType.image_jpeg:
                    return "image/jpeg";
                case ContentType.image_png:
                    return "image/png";
                case ContentType.image_svg_xml:
                    return "image/svg+xml";
                case ContentType.image_tiff:
                    return "image/tiff";
                case ContentType.image_webp:
                    return "image/webp";
                case ContentType.application_x_csh:
                    return "application/x-csh";
                case ContentType.application_msword:
                    return "application/msword";
                case ContentType.application_vnd_openxmlformats_officedocument_wordprocessingml_document:
                    return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
                case ContentType.text_html:
                    return "text/html";
                case ContentType.text_javascript:
                    return "text/javascript";
                case ContentType.application_json:
                    return "application/json";
                case ContentType.application_ld_json:
                    return "application/ld+json";
                case ContentType.application_vnd_ms_powerpoint:
                    return "application/vnd.ms-powerpoint";
                case ContentType.application_vnd_openxmlformats_officedocument_presentationml_presentation:
                    return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
                case ContentType.application_xhtml_xml:
                    return "application/xhtml+xml";
                case ContentType.application_vnd_openxmlformats_officedocument_spreadsheetml_sheet:
                    return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
                case ContentType.text_csv:
                    return "text/csv";
                case ContentType.audio_basic:
                    return "audio/basic";
                case ContentType.application_pdf:
                    return "application/pdf";                
                default:
                    // Handle bad URL, possibly throw
                    throw new Exception();
            }
        }
        
}
}
