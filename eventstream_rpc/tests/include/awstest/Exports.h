#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#if defined(AWS_ECHOTESTRPC_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_ECHOTESTRPC_USE_IMPORT_EXPORT
#        ifdef AWS_ECHOTESTRPC_EXPORTS
#            define AWS_ECHOTESTRPC_API __declspec(dllexport)
#        else
#            define AWS_ECHOTESTRPC_API __declspec(dllimport)
#        endif /* AWS_ECHOTESTRPC_EXPORTS */
#    else
#        define AWS_ECHOTESTRPC_API
#    endif /* AWS_ECHOTESTRPC_USE_IMPORT_EXPORT */

#else /* defined (AWS_ECHOTESTRPC_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_ECHOTESTRPC_API
#endif /* defined (AWS_ECHOTESTRPC__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
