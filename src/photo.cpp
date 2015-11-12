#include <stdio.h>

#include <curl/curl.h>

#include <photo.h>

Photo::Photo(std::string id, std::string link)
{
  this->id.assign(id);
  this->link.assign(link);
}

std::string Photo::getFilePath(void)
{
  std::string file_path = PHOTOBASE_ROOT;
  file_path += "/";
  file_path += id;

  return file_path;
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
  const char *outfilename = getFilePath().c_str();
  curl = curl_easy_init();
  if (curl) {
    fp = fopen(outfilename,"wb");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    res = curl_easy_perform(curl);
    /* always cleanup */
    curl_easy_cleanup(curl);
    fclose(fp);
  }
  return 0;
}
