#include "Post.hpp"

Post::Post(std::string path, std::string content): Method(path, content) {}

Post::~Post() {}

void	Post::execute_method() {}