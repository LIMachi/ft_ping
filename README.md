Subject: https://cdn.intra.42.fr/pdf/pdf/124498/en.subject.pdf \
Rewrite a minimal copy of the ping command in C

Reference used:
- inetutils' ping: https://manpages.debian.org/stretch/inetutils-ping/ping.1.en.html
- standard deviation: https://en.wikipedia.org/wiki/Standard_deviation#Rapid_calculation_methods \ https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
- ping: https://en.wikipedia.org/wiki/Ping_(networking_utility)
- icmp: https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
- checksum: https://en.wikipedia.org/wiki/Internet_checksum

Libraries used:
- Math (sqrt function)

Completion:
- [x] timer system
- [x] print system
- [x] ping message
- [x] socket
- [ ] verbose option (missing: listing of unexpected packets)
- [x] quiet option

Bonus:
- [x] 1 allocation (the only one present is called by `getaddrinfo` and cannot be avoided)
- [ ] s/packet-size option
- [ ] l/preload option
- [x] i/interval option
- [x] c/count option
- [x] t/ttl option