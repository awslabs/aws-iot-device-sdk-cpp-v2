#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#if defined(AWS_GREENGRASSIPC_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_GREENGRASSIPC_USE_IMPORT_EXPORT
#        ifdef AWS_GREENGRASSIPC_EXPORTS
#            define AWS_GREENGRASSIPC_API __declspec(dllexport)
#        else
#            define AWS_GREENGRASSIPC_API __declspec(dllimport)
#        endif /* AWS_GREENGRASSIPC_EXPORTS */
#    else
#        define AWS_GREENGRASSIPC_API
#    endif /* AWS_GREENGRASSIPC_USE_IMPORT_EXPORT */

#else /* defined (AWS_GREENGRASSIPC_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_GREENGRASSIPC_API
#endif /* defined (AWS_GREENGRASSIPC__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
