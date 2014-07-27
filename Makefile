TARGET		:=pro_bonus
OBJS			:=main.o lex.o syntactic.o netlist.o parse.o simulate.o
HEADER		:=lex.h syntactic.h netlist.h parse.h simulate.h
CC				:=g++
$(TARGET):$(OBJS)
	$(CC)  -o $@ $^
$(OBJS):%.o:%.cpp $(HEADER)
	$(CC) -g -c $<
.PHONY : clean clear send  do test
clean:
	-rm $(TARGET) $(OBJS)
clear:
	-cd ../golden/bonus/ && rm -f *.evl.sim_out *.evl.tokens *.evl.syntax *.evl.statements *.evl.netlist
send:
	-cp -f $(TARGET)  ../golden/bonus
do:
	./$(TARGET) tris_lut_assign.evl
test:
	-cd ../golden/bonus/ && ./$(TARGET) tris_lut_assign.evl
	-cd ../golden/ && ./EasyVL ./bonus/tris_lut_assign.evl proj=BONUS testing=true
