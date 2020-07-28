#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotjobs/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

namespace Aws
{
    namespace Iotjobs
    {

        enum class RejectedErrorCode
        {
            InvalidTopic,
            InvalidJson,
            InvalidRequest,
            InvalidStateTransition,
            ResourceNotFound,
            VersionMismatch,
            InternalError,
            RequestThrottled,
            TerminalStateReached,
        };

        namespace RejectedErrorCodeMarshaller
        {
            AWS_IOTJOBS_API const char *ToString(RejectedErrorCode val);
            AWS_IOTJOBS_API RejectedErrorCode FromString(const Aws::Crt::String &val);
        } // namespace RejectedErrorCodeMarshaller
    }     // namespace Iotjobs
} // namespace Aws
