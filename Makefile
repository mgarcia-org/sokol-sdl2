TARGETS = 	deforming_offscreen instancing mrt mipmap blend arraytex \
			inject dyntex offscreen \
		  	texcube triangle cube


LLDLIBS = -Wall -lSDL2 -lGL -lGLU -lm # $(GLUT) -lGLU -lGL -lXext -lX11  #-lXmu 

all:  $(TARGETS) makebin

	 
makebin:	$(TARGETS)
	chmod 777 bin/*.bin
	
$(TARGETS):	
	echo $@.o
	gcc  -g -Wfatal-errors -I/usr/include -I.   $@.c  -o bin/$@.bin $(LLDLIBS)
	
	


clean:  
	-rm -f bin/*.bin
