#pragma once

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif


inline String file_to_string(String$& file_path) {
  let file = fopen(file_path.c_str(), "r");
  
  if (file == nullptr)
    THROW("This file could not be opened: \""+file_path+"\". "
          "Did you type it right?");
  
  fseek(file, 0, SEEK_END);
  let file_size = ftell(file);
  rewind(file);

  var file_contents = new char[file_size];
  afterwards { delete[] file_contents; };
  fread(file_contents, sizeof(u8), file_size, file);

  let str = String(file_contents);
  return str;
}
