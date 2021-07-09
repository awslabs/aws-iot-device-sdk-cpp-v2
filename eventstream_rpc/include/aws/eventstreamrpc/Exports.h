#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#if defined(AWS_EVENTSTREAMRPC_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_EVENTSTREAMRPC_USE_IMPORT_EXPORT
#        ifdef AWS_EVENTSTREAMRPC_EXPORTS
#            define AWS_EVENTSTREAMRPC_API __declspec(dllexport)
#        else
#            define AWS_EVENTSTREAMRPC_API __declspec(dllimport)
#        endif /* AWS_EVENTSTREAMRPC_EXPORTS */
#    else
#        define AWS_EVENTSTREAMRPC_API
#    endif /* AWS_EVENTSTREAMRPC_USE_IMPORT_EXPORT */

#else /* defined (AWS_EVENTSTREAMRPC_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_EVENTSTREAMRPC_API
#endif /* defined (AWS_EVENTSTREAMRPC__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
