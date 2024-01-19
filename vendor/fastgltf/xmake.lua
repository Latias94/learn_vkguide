add_requires("simdjson")

target("fastgltf")
    set_kind("static")
    add_files("src/*.cpp")
    add_includedirs("include", {public = true})
    add_packages("simdjson", {public = true})
