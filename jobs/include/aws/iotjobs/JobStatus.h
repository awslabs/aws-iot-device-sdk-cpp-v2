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

        enum class JobStatus
        {
            QUEUED,
            IN_PROGRESS,
            TIMED_OUT,
            FAILED,
            SUCCEEDED,
            CANCELED,
            REJECTED,
            REMOVED,
        };

        namespace JobStatusMarshaller
        {
            AWS_IOTJOBS_API const char *ToString(JobStatus val);
            AWS_IOTJOBS_API JobStatus FromString(const Aws::Crt::String &val);
        } // namespace JobStatusMarshaller
    }     // namespace Iotjobs
} // namespace Aws
