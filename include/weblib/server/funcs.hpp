// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy

#pragma once
#include <functional>

class HttpRequest;
class HttpResponseWriter;
using Handler = std::function<void(const HttpRequest&, HttpResponseWriter&)>;
using Filter = std::function<bool(const HttpRequest&)>;