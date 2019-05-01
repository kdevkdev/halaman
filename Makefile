# from 

CXX = g++

objdir = obj
srcdir = src
bindir = makebin

fileend = .cpp

src = $(wildcard $(srcdir)/*.cpp)
#obj = $(src:.cpp=.o)
obj = $(src:$(srcdir)/%.cpp=$(objdir)/%.o) 

$(info $(src))
$(info $(obj))

	
CFLAGS  = -Wall -m64 -march=corei7-avx -fomit-frame-pointer -fexpensive-optimizations -O3 -std=c++14 -m64 -DNDEBUG
LDFLAGS = -lfreetype -lSDL2 -lGL -lGLEW -lcppunit -lfreetype -s -m64
INC  = -I/usr/include/freetype2 -Iinc -Isrc

halaman: $(obj)
	@mkdir -p $(bindir)
	$(CXX)  -o $(bindir)/$@ $^ $(LDFLAGS)
	cp -r resources $(bindir)

	

$(objdir)/%.o: $(srcdir)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CFLAGS) $(INC)

    
.PHONY: clean
clean:
	rm -f $(obj) halaman
	rm -rf $(objdir)
	rm -rf $(bindir)


