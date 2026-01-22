#
# Copyright (C) 2023  Autodesk, Inc. All Rights Reserved.
#
# SPDX-License-Identifier: Apache-2.0
#
import os


def __dummy__():
    pass


QWebEngineProfile = None

try:
    from PySide2.QtWebEngineWidgets import QWebEngineProfile
except ImportError:
    try:
        from PySide6.QtWebEngineCore import QWebEngineProfile
    except ImportError:
        QWebEngineProfile = None


def setup_webview_default_profile():
    if QWebEngineProfile is None:
        return
    default_profile = QWebEngineProfile.defaultProfile()
    user_agent = default_profile.httpUserAgent() + " RV/" + os.environ["TWK_APP_VERSION"]
    default_profile.setHttpUserAgent(user_agent)


setup_webview_default_profile()
