#pragma once


inline String file_to_string(String$& file_path) {
  let file = fopen(file_path.c_str(), "r");
  
  if (file == nullptr)
    THROW("This file could not be opened: \""+file_path+"\". "
          "Did you type it right?");
  
  fseek(file, 0, SEEK_END);
  let file_size = ftell(file);
  rewind(file);
  
  String file_contents;
  file_contents.resize(file_size);
  fread(file_contents.data(), sizeof(u8), file_size, file);
  
  return file_contents;
}