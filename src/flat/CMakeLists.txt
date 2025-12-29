
set(flat_path ${CMAKE_SOURCE_DIR})
# 获取所有的fbs文件,
file(GLOB files "*.fbs")

# 生成的文件
set(fb_h "")

foreach(file ${files})
    string(REPLACE ".fbs" "_fb.h" fb_h ${file})
    if(NOT EXISTS ${fb_h})
        # 如果没有就生成falt头文件
        execute_process(
            COMMAND flatc --cpp -o ${flat_path} --filename-suffix _fb ${file}
        )
    endif()
endforeach()
