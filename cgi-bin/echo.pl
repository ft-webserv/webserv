#!/usr/bin/perl
use CGI qw(:standard);

my $http_status_code = '200 OK';
my $html_content = '';

if ($ENV{'REQUEST_METHOD'} eq 'POST') {
  while (<STDIN>) {
    $html_content .= $_;
  }
} else {
  $html_content = <<END_HTML;
<!DOCTYPE html>
<html>
<body>
  <h1>echo</h1>
  <p>perl로 작성된 echo프로그램입니다. post요청시 body를 반환합니다.</p>
</body>
</html>
END_HTML
}

print header(-status => $http_status_code);
print "$html_content";
