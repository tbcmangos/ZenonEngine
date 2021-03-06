/*
   Copyright (c) 2012-2016 Alex Zhondin <lexxmark.dev@gmail.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "PropertyInt.h"

bool QtnPropertyIntBase::fromStrImpl(const QString& str)
{
    bool ok = false;
    ValueType value = str.toInt(&ok);
    if (!ok)
        return false;

    setValue(value);
    return true;
}

bool QtnPropertyIntBase::toStrImpl(QString& str) const
{
    str = QString::number(value());
    return true;
}
