set_project("learn_vkguide")
set_languages("c++23")

add_rules("mode.debug", "mode.release")

set_exceptions("cxx")

includes("app/xmake.lua")
includes("engine/xmake.lua")
includes("vendor/**/xmake.lua")
