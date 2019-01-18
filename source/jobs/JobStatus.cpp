/*
* Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.
*/
#include <aws/iotsdk/jobs/JobStatus.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/StringUtils.h>

static const size_t QUEUED_HASH = Aws::Crt::HashString("QUEUED");
static const size_t IN_PROGRESS_HASH = Aws::Crt::HashString("IN_PROGRESS");
static const size_t FAILED_HASH = Aws::Crt::HashString("FAILED");
static const size_t SUCCESS_HASH = Aws::Crt::HashString("SUCCESS");
static const size_t CANCELED_HASH = Aws::Crt::HashString("CANCELED");
static const size_t REJECTED_HASH = Aws::Crt::HashString("REJECTED");
static const size_t REMOVED_HASH = Aws::Crt::HashString("REMOVED");

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            namespace JobStatusMarshaller
            {
                const char* ToString(JobStatus status)
                {
                    switch (status)
                    {
                    case JobStatus::QUEUED:
                        return "QUEUED";
                    case JobStatus::IN_PROGRESS:
                        return "IN_PROGRESS";
                    case JobStatus::FAILED:
                        return "FAILED";
                    case JobStatus::SUCCESS:
                        return "SUCCESS";
                    case JobStatus::CANCELED:
                        return "CANCELED";
                    case JobStatus::REJECTED:
                        return "REJECTED";
                    case JobStatus::REMOVED:
                        return "REMOVED";
                    default:
                        assert(0);
                        return "UNKNOWN_STATUS";
                    }
                }

                JobStatus FromString(const Crt::String& str)
                {
                    size_t hash = Crt::HashString(str.c_str());

                    if (hash == QUEUED_HASH)
                    {
                        return JobStatus::QUEUED;
                    }

                    if (hash == IN_PROGRESS_HASH)
                    {
                        return JobStatus::IN_PROGRESS;
                    }

                    if (hash == FAILED_HASH)
                    {
                        return JobStatus::FAILED;
                    }

                    if (hash == SUCCESS_HASH)
                    {
                        return JobStatus::SUCCESS;
                    }

                    if (hash == CANCELED_HASH)
                    {
                        return JobStatus::CANCELED;
                    }

                    if (hash == REJECTED_HASH)
                    {
                        return JobStatus::REJECTED;
                    }

                    if (hash == REMOVED_HASH)
                    {
                        return JobStatus::REMOVED;
                    }

                    assert(0);
                    return static_cast<JobStatus>(-1);
                }
            }
        }
    }
}