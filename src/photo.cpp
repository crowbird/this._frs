#include <stdio.h>
#include <fstream>
#include <sys/stat.h>

#include <curl/curl.h>

#include <photo.h>

Photo::Photo(std::string id, std::string link, int face_count)
{
  processed = false;
  valid = false;
  faces = new Face*[face_count];
  for (int face_idx = 0; face_idx < face_count; face_idx++)
  {
    faces[face_idx] = NULL;
  }
  // add when faces are added
  this->face_count = 0;
  this->id.assign(id);
  this->link.assign(link);
}

std::ifstream::pos_type filesize(const char* filename)
{
  std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  return in.tellg(); 
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t written = fwrite(ptr, size, nmemb, stream);
  return written;
}

int Photo::downloadLink(void)
{
  CURL *curl;
  FILE *fp;
  CURLcode res;
  const char *url = link.c_str();

  file_path = PHOTOBASE_ROOT;
  file_path.append("/");
  file_path.append(id);

  struct stat buffer;
  if ((stat (file_path.c_str(), &buffer) == 0) && int(filesize(file_path.c_str())) != 0)
  {
    downloaded = true;
    return 0;
  }

  const char *outfilename = file_path.c_str();

  curl = curl_easy_init();
  if (curl) {
    fp = fopen(outfilename,"wb");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    printf("downloading %s...\n", outfilename);
    res = curl_easy_perform(curl);
    /* always cleanup */
    curl_easy_cleanup(curl);
    fclose(fp);
  }
  else
  {
    printf("failed to init curl\n");
  }
  if ((stat (file_path.c_str(), &buffer) == 0) && int(filesize(file_path.c_str())) != 0)
  {
    downloaded = true;
    return 0;
  }
  else
  {
    printf("failed to download photo\n");
  }
  return 0;
}
