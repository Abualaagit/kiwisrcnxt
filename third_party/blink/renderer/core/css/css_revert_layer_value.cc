// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/css/css_revert_layer_value.h"

#include "third_party/blink/renderer/core/css/css_value_pool.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {
namespace cssvalue {

CSSRevertLayerValue* CSSRevertLayerValue::Create() {
  return CssValuePool().RevertLayerValue();
}

String CSSRevertLayerValue::CustomCSSText() const {
  return "revert-layer";
}

}  // namespace cssvalue
}  // namespace blink
