/* Copyright 2010-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

* This file is generated
*/
#include <aws/iotshadow/DeleteShadowRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void DeleteShadowRequest::LoadFromObject(DeleteShadowRequest &val, const Aws::Crt::JsonView &doc) {}

        void DeleteShadowRequest::SerializeToObject(Aws::Crt::JsonObject &object) const {}

        DeleteShadowRequest::DeleteShadowRequest(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        DeleteShadowRequest &DeleteShadowRequest::operator=(const Crt::JsonView &doc)
        {
            *this = DeleteShadowRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws