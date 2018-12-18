#pragma once
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
#include <aws/iotsdk/jobs/DescribeJobExecutionRequest.h>
#include <aws/iotsdk/jobs/DescribeJobExecutionResponse.h>
#include <aws/iotsdk/jobs/DescribeJobExecutionSubscriptionRequest.h>

#include <aws/iotsdk/jobs/JobsError.h>

#include <aws/crt/DateTime.h>
#include <aws/crt/Types.h>
#include <aws/crt/StlAllocator.h>

#include <aws/crt/mqtt/MqttClient.h>

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            using OnSubscribeComplete = std::function<void(int ioErr)>;
            using OnDescribeJobExecutionAcceptedResponse =
                    std::function<void(DescribeJobExecutionResponse*, int ioErr)>;

            using OnDescribeJobExecutionRejectedResponse =
                std::function<void(JobsError*, int ioErr)>;

            using OnPublishComplete = std::function<void(int ioErr)>;

            class IotJobsClient final
            {
            public:
                IotJobsClient(const std::shared_ptr<Crt::Mqtt::MqttConnection>& connection);

                operator bool() const noexcept;
                int GetLastError() const noexcept;

                bool SubscribeToDescribeJobExecutionAccepted(const DescribeJobExecutionSubscriptionRequest& request, 
                    Crt::Mqtt::QOS qos, const OnDescribeJobExecutionAcceptedResponse& handler, const OnSubscribeComplete& onSubAckHandler);
                bool SubscribeToDescribeJobExecutionRejected(const DescribeJobExecutionSubscriptionRequest& request, 
                    Crt::Mqtt::QOS qos, const OnDescribeJobExecutionRejectedResponse& handler, const OnSubscribeComplete& onSubAckHandler);
                bool PublishDescribeJobExecution(const DescribeJobExecutionRequest& request,
                        Crt::Mqtt::QOS qos, const OnPublishComplete& handler);

            private:
                std::shared_ptr<Crt::Mqtt::MqttConnection> m_connection;
            };
        }
    }
}