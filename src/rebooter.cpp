//============================================================================
// Name        : rebooter.cpp
// Author      : moriarty
// Version     :
// Copyright   : All rights reserved
// Description : Hello World in C++, Ansi-style
//============================================================================
// add to crontab *1 * * * * /rebooter

#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

class Mediator;
class Object;


using namespace std;

class Mediator
{
public:
	bool is_calendar;
	virtual void check( bool message, Object *object) = 0;
};

class Object
{
protected:
        Mediator* mediator_;

public:
        explicit Object(Mediator *mediator):mediator_(mediator)
        {
        }
};

class Calendar:public Object
{
public:
    explicit Calendar(Mediator *mediator):Object(mediator)
    {
    }

    void check()
    {
		time_t rawtime;
		struct tm * timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		// [0, 1, 2, 3, 4, 5, 6] = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]
		mktime ( timeinfo );
		if(timeinfo->tm_wday==1 || timeinfo->tm_wday==0 )
		{
			mediator_->check(true,this);
			mediator_->is_calendar =true;
		}
    }

	void action()
	{
	}
};

class Notify:public Object
{
public:
    explicit Notify(Mediator *mediator):Object(mediator)
    {
    }

	void check()
	{
		time_t rawtime;
		struct tm * ptm;
		time ( &rawtime );
		ptm = gmtime ( &rawtime );
		if(ptm->tm_hour == 4 && ptm->tm_min == 55)
		{
			mediator_->check(true,this);
		}
	}

	void action()
	{
		cout << "It will reboot after 5 min" << endl;
	}
};

class Backup:public Object
{

public:
	explicit Backup(Mediator *mediator):Object(mediator)
	{
	}

	void check()
	{
		time_t rawtime;
		struct tm * ptm;
		time ( &rawtime );
		ptm = gmtime ( &rawtime );
		if(ptm->tm_hour == 4 && ptm->tm_min == 30)
		{
			mediator_->check(true,this);
		}
	}

	void action()
	{
		system("cp -rp /home /Backup");
		cout << "Data backup is end!" << endl;
	}
};

class Reboot:public Object
{
public:
	explicit Reboot(Mediator *mediator):Object(mediator)
	{
	}

	void check()
	{
		time_t rawtime;
		struct tm * ptm;
		time ( &rawtime );
		ptm = gmtime ( &rawtime );
		if(ptm->tm_hour == 5 && ptm->tm_min == 0)
		{
			mediator_->check(true,this);
		}
	}

	void action()
	{
		cout << "Rebooting" << endl;
		system("reboot");
	}
};

class CommonMediator:public Mediator
{
protected:
	Notify *notify;
	Backup *backup;
	Reboot *reboot;
public:
    void SetNotify(Notify *n)
    {
    	notify=n;
    }
    void SetBackup(Backup *b)
    {
    	backup=b;
    }
    void SetReboot(Reboot *r)
    {
    	reboot=r;
    }
	virtual void check(bool message, Object *object)
	{
		if(message && object==notify)
		{
			notify->action();
		}
		if(message && object==backup)
		{
			backup->action();
		}
		if(message && object==reboot)
		{
			reboot->action();
		}
	}

};

int main() {
	CommonMediator mediator;

	Calendar c(&mediator);
	Notify n(&mediator);
	Backup b(&mediator);
	Reboot r(&mediator);

	mediator.SetNotify(&n);
	mediator.SetBackup(&b);
	mediator.SetReboot(&r);

	c.check();
	if(mediator.is_calendar)
	{
		n.check();
		b.check();
		r.check();
	}

	cin.get();
	return 0;
}
