#ifndef STATELIGHT_H
#define STATELIGHT_H

#pragma once

enum MY_COLOR { RED, GREEN, GRAY };

const QString grayStateLightStyleSheet =
    "border:1px; border-style:solid; border-color: #b4b4b4; background:#F6F6F6; min-width:30px; max-width:30px; "
    "min-height:30px; max-height:30px; border-radius: 15px;";
const QString greenStateLightStyleSheet =
    "border:1px; border-style:solid; border-color: #b4b4b4; background:#00FF00; min-width:30px; max-width:30px; "
    "min-height:30px; max-height:30px; border-radius: 15px;";
const QString redStateLightStyleSheet =
    "border:1px; border-style:solid; border-color: #b4b4b4; background:#FF0000; min-width:30px; max-width:30px; "
    "min-height:30px; max-height:30px; border-radius: 15px;";

#endif  // STATELIGHT_H
