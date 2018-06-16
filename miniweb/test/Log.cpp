#ifndef _LOG_H_
#define _LOG_H_
#endif

void LOG::write_log(LOG log)
{
    string path = log.log_path + log.log_name;
    fstream file(path);
    file >> log.s >> '\r\n';
}
