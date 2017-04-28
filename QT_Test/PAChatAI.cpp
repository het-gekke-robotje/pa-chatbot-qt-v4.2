#include <vector>
#include <tuple>
#include <functional>
#include "PAChatAI.h"

//random question, random comment after answer received, random adders, is this an possible answer? if yes how to process answer, false = next chat
using qp = std::tuple< 
	QStringList, // 0 - questions
	QStringList, // 1 - reactions to answers to questions
	QStringList, // 2 - random adder to questions
	QStringList, // 3 - random adder to answers
	std::function<bool(QString)>, // 4 - check if this is an possible answer, false=ignore, true=invoke answer processing
	std::function<bool(QString,bool*,int*)>,  // 5 - process answer, false = disconnect chat, true = next question
	int, // 6 - the question ask time
	int // 7 - time for typing reaction
>;

QString SmartReplaceNumbers(QString input)
{
	using nm = std::pair<QString, int>;

	static const std::vector<nm> number_replace =
	{
		nm("een", 1),
		nm("twee", 2),
		nm("drie", 3),

		nm("vier", 4),
		nm("veer", 4),
		nm("fier", 4),

		nm("vijf", 5),
		nm("fijf", 5),
		nm("fijv", 5),
		nm("vijv", 5),
		nm("veif", 5),
		nm("feif", 5),
		nm("feiv", 5),
		nm("veiv", 5),

		nm("zes", 6),
		nm("ses", 6),
		nm("zez", 6),
		nm("sez", 6),

		nm("zeven", 7),
		nm("seven", 7),
		nm("sven", 7),
		nm("zven", 7),
		nm("zvn", 7),
		nm("svn", 7),

		nm("acht", 8),
		nm("agt", 8),
		nm("achd", 8),
		nm("agd", 8),

		nm("negen", 9),
		nm("neger", 9),
		nm("ngen", 9),
		nm("negn", 9),
		nm("nein", 9),
	};

	static const std::vector<nm> number_tens_replace =
	{
		nm("elf", 11),
		nm("elv", 11),
		nm("twaal", 12),
		nm("twal", 12),
		nm("dertien", 13),

		nm("tien", 10),

		nm("twin", 20),
		nm("twnt", 20),

		nm("dertig", 30),

		nm("veer", 40),

		nm("vijftig", 50),
		nm("fijftig", 50),
		nm("fijvtig", 50),
		nm("vijvtig", 50),
		nm("veiftig", 50),
		nm("feiftig", 50),
		nm("feivtig", 50),
		nm("veivtig", 50),

		nm("zestig", 60),
		nm("sestig", 60),
		nm("zeztig", 60),
		nm("seztig", 60),

		nm("zeventig", 70),
		nm("seventig", 70),
		nm("sventig", 70),
		nm("zventig", 70),
		nm("zvntig", 70),
		nm("svntig", 70),

		nm("achtig", 80),
		nm("agtig", 80),
		nm("achdig", 80),
		nm("agdig", 80),
		nm("achttig", 80),
		nm("agttig", 80),
		nm("achdtig", 80),
		nm("agdtig", 80),

		nm("negentig", 90),
		nm("negertig", 90),
		nm("ngentig", 90),
		nm("negntig", 90),
		nm("neintig", 90),

		nm("vijftg", 50),
		nm("fijftg", 50),
		nm("fijvtg", 50),
		nm("vijvtg", 50),
		nm("veiftg", 50),
		nm("feiftg", 50),
		nm("feivtg", 50),
		nm("veivtg", 50),

		nm("zestg", 60),
		nm("sestg", 60),
		nm("zeztg", 60),
		nm("seztg", 60),

		nm("zeventg", 70),
		nm("seventg", 70),
		nm("sventg", 70),
		nm("zventg", 70),
		nm("zvntg", 70),
		nm("svntg", 70),

		nm("achtg", 80),
		nm("agtg", 80),
		nm("achdig", 80),
		nm("agdig", 80),
		nm("achttg", 80),
		nm("agttg", 80),
		nm("achdtg", 80),
		nm("agdtg", 80),

		nm("negentg", 90),
		nm("negertg", 90),
		nm("ngentg", 90),
		nm("negntg", 90),
		nm("neintg", 90),

		nm("vijftich", 50),
		nm("fijftich", 50),
		nm("fijvtich", 50),
		nm("vijvtich", 50),
		nm("veiftich", 50),
		nm("feiftich", 50),
		nm("feivtich", 50),
		nm("veivtich", 50),

		nm("zestich", 60),
		nm("sestich", 60),
		nm("zeztich", 60),
		nm("seztich", 60),

		nm("zeventich", 70),
		nm("seventich", 70),
		nm("sventich", 70),
		nm("zventich", 70),
		nm("zvntich", 70),
		nm("svntich", 70),

		nm("achtich", 80),
		nm("agtich", 80),
		nm("achdich", 80),
		nm("agdich", 80),
		nm("achttich", 80),
		nm("agttich", 80),
		nm("achdtich", 80),
		nm("agdtich", 80),

		nm("negentich", 90),
		nm("negertich", 90),
		nm("ngentich", 90),
		nm("negntich", 90),
		nm("neintich", 90),

		nm("vijftch", 50),
		nm("fijftch", 50),
		nm("fijvtch", 50),
		nm("vijvtch", 50),
		nm("veiftch", 50),
		nm("feiftch", 50),
		nm("feivtch", 50),
		nm("veivtch", 50),

		nm("zestch", 60),
		nm("sestch", 60),
		nm("zeztch", 60),
		nm("seztch", 60),

		nm("zeventch", 70),
		nm("seventch", 70),
		nm("sventch", 70),
		nm("zventch", 70),
		nm("zvntch", 70),
		nm("svntch", 70),

		nm("achtch", 80),
		nm("agtch", 80),
		nm("achdich", 80),
		nm("agdich", 80),
		nm("achttch", 80),
		nm("agttch", 80),
		nm("achdtch", 80),
		nm("agdtch", 80),

		nm("negentch", 90),
		nm("negertch", 90),
		nm("ngentch", 90),
		nm("negntch", 90),
		nm("neintch", 90),
	};

	int number = 0;
	for (auto& replacer : number_tens_replace)
	{
		if (input.indexOf(replacer.first) != -1)
		{
			input = input.replace(replacer.first, "");
			number += replacer.second;
		}
	}

	for (auto& replacer : number_replace)
	{
		if (input.indexOf(replacer.first) != -1)
		{
			input = input.replace(replacer.first, "");
			number += replacer.second;
		}
	}

	input = input.append(" (number: " + QString::number(number) + " )");

	return input;
}

std::vector<qp> questions_answers =
{
	qp(
		QStringList{ 
			"j/m?", "jm?", "jm?", "jm?", "Jm?", "jm?", "JM?", "jm?", "J/m?", "jm?", "J/M?", "jm?", "jongen/meisje?", "jm?", "jongen of meisje?", "jm?", "j m?", "jm?", "J m?", "jm?", "J M?", "jm?", "j M?", "jm?", "jongen/meid?", "jm?", 
			"hoi, j/m?", "jm?", "hoi, jm?", "jm?", "hoi, Jm?", "jm?", "hoi, JM?", "jm?", "hoi, J/m?", "jm?", "hoi, J/M?", "jm?", "hoi, jongen/meisje?", "jm?", "hoi, jongen of meisje?", "jm?", "hoi, j m?", "jm?", "hoi, J m?", "jm?", "hoi, J M?", "jm?", "hoi, j M?", "jm?", "hoi, jongen/meid?", "jm?",
			"hoi j/m?", "jm?", "hoi jm?", "jm?", "hoi Jm?", "jm?", "hoi JM?", "jm?", "hoi J/m?", "jm?", "hoi J/M?", "jm?", "hoi jongen/meisje?", "jm?", "hoi jongen of meisje?", "jm?", "hoi j m?", "jm?", "hoi J m?", "jm?", "hoi J M?", "jm?", "hoi j M?", "jm?", "hoi jongen/meid?", "jm?",
			"hey, j/m?", "jm?", "hey, jm?", "jm?", "hey, Jm?", "jm?", "hey, JM?", "jm?", "hey, J/m?", "jm?", "hey, J/M?", "jm?", "hey, jongen/meisje?", "jm?", "hey, jongen of meisje?", "jm?", "hey, j m?", "jm?", "hey, J m?", "jm?", "hey, J M?", "jm?", "hey, j M?", "jm?", "hey, jongen/meid?", "jm?",
			"hey j/m?", "jm?", "hey jm?", "jm?", "hey Jm?", "jm?", "hey JM?", "jm?", "hey J/m?", "jm?", "hey J/M?", "jm?", "hey jongen/meisje?", "jm?", "hey jongen of meisje?", "jm?", "hey j m?", "jm?", "hey J m?", "jm?", "hey J M?", "jm?", "hey j M?", "jm?", "hey jongen/meid?", "jm?",
			"haai, j/m?", "jm?", "haai, jm?", "jm?", "haai, Jm?", "jm?", "haai, JM?", "jm?", "haai, J/m?", "jm?", "haai, J/M?", "jm?", "haai, jongen/meisje?", "jm?", "haai, jongen of meisje?", "jm?", "haai, j m?", "jm?", "haai, J m?", "jm?", "haai, J M?", "jm?", "haai, j M?", "jm?", "haai, jongen/meid?", "jm?",
			"haai j/m?", "jm?", "haai jm?", "jm?", "haai Jm?", "jm?", "haai JM?", "jm?", "haai J/m?", "jm?", "haai J/M?", "jm?", "haai jongen/meisje?", "jm?", "haai jongen of meisje?", "jm?", "haai j m?", "jm?", "haai J m?", "jm?", "haai J M?", "jm?", "haai j M?", "jm?", "haai jongen/meid?", "jm?",
			"hooi, j/m?", "jm?", "hooi, jm?", "jm?", "hooi, Jm?", "jm?", "hooi, JM?", "jm?", "hooi, J/m?", "jm?", "hooi, J/M?", "jm?", "hooi, jongen/meisje?", "jm?", "hooi, jongen of meisje?", "jm?", "hooi, j m?", "jm?", "hooi, J m?", "jm?", "hooi, J M?", "jm?", "hooi, j M?", "jm?", "hooi, jongen/meid?", "jm?",
			"hooi j/m?", "jm?", "hooi jm?", "jm?", "hooi Jm?", "jm?", "hooi JM?", "jm?", "hooi J/m?", "jm?", "hooi J/M?", "jm?", "hooi jongen/meisje?", "jm?", "hooi jongen of meisje?", "jm?", "hooi j m?", "jm?", "hooi J m?", "jm?", "hooi J M?", "jm?", "hooi j M?", "jm?", "hooi jongen/meid?"
		},
		QStringList{ "ik J", "ik jongen", "jongen hier", "j hier", "hier j", "hiero j" },
		QStringList{ "", " :)", "", " :P", "", " ;)", "", " :D", "", " ;D", "", " ;P", "" },
		QStringList{ 
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21 :)", " bijna 21 :)", " hihi 21 :)", " haha 21 :)", ", al een tijdje 21 :)",
			"21 :P", " bijna 21 :P", " hihi 21 :P", " haha 21 :P", ", al een tijdje 21 :P",
			"21 ;)", " bijna 21 ;)", " hihi 21 ;)", " haha 21 ;)", ", al een tijdje 21 ;)",
			"21 :D", " bijna 21 :D", " hihi 21 :D", " haha 21 :D", ", al een tijdje 21 :D",
			"21 ;D", " bijna 21 ;D", " hihi 21 ;D", " haha 21 ;D", ", al een tijdje 21 ;D",
			"21 ;P", " bijna 21 ;P", " hihi 21 ;P", " haha 21 ;P", ", al een tijdje 21 ;P",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21",
			"21", " bijna 21", " hihi 21", " hehe 21", ", al een tijdje 21"
		},
		[&](QString answer)
		{

			const static QStringList possible_answers = { "d", "m", "v", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
			for (auto& possible_answer : possible_answers)
			{
				if (answer.indexOf(possible_answer) != -1)
				{
					return true;
				}
			}
			return false;
		},
		[&](QString answer, bool* got_age, int* determined_age)
		{
			//filter already filters this, try determine age here
			QString s_number;
			int digits = 0;
			for (auto& c : answer)
			{
				if (c.isDigit())
				{
					s_number += c;
					++digits;
				}
			}
			bool ok;
			int age = s_number.toInt(&ok);
			if (got_age)
			{
				*got_age = digits != 0 && ok;
				if (determined_age)
				{
					*determined_age = age;
				}
			}

			return digits == 0 || (ok && age > 15 && age < 21);
		},
		2000,
		3000
	),

	qp(
		QStringList{ "en jouw leeftijd?", "en wat is je leeftijd?", "en hoe oud ben jij?", "en hoeveel jaartjes heb jij erop zitten?", "en je leeftijd?", "en jou leeftijd?" },
		QStringList{ "" },
		QStringList{ "", " :)", "", " :P", "", " ;)", "", " :D", "", " ;D", "", " ;P", "" },
		QStringList{ "", " :)", "", " :P", "", " ;)", "", " :D", "", " ;D", "", " ;P", "" },
		[&](QString answer)
		{
			answer = SmartReplaceNumbers(answer);
			for (auto& c : answer)
			{
				if (c.isDigit())
				{
					return true;
				}
			}
			return false;
		},
		[&](QString answer, bool*, int*)
		{
			QString s_number;
			answer = SmartReplaceNumbers(answer);
			for (auto& c : answer)
			{
				if (c.isDigit())
				{
					s_number += c;
				}
			}
			bool ok;
			int age = s_number.toInt(&ok);
			return ok && age > 15 && age < 21;
		},
		3000,
		2000
	)
};

PAChatAI::PAChatAI(QObject *parent)
	: QObject(parent)
{
	filter_ = new PAChatClientFilter(this);

	connect(&question_akser_, &QTimer::timeout, this, &PAChatAI::onAskNextQuestion);
	connect(&reaction_sender_, &QTimer::timeout, this, &PAChatAI::onReactionToAnswer);
	connect(&waiting_answer_timeout_, &QTimer::timeout, this, &PAChatAI::onWaitForAnswerTimeout);
}

PAChatAI::~PAChatAI()
{

}

void PAChatAI::Start()
{
	current_index_ = 0;
	amount_incomming_messages_ = 0;

	got_age_ = false;
	AskNextQuestion();
}

void PAChatAI::Stop()
{
	waiting_answer_timeout_.stop();
	question_akser_.stop();
	reaction_sender_.stop();
	state_ = PAChatAIState_Done;
}

bool PAChatAI::Stopped()
{
	return 
		!question_akser_.isActive() && 
		!reaction_sender_.isActive() && 
		!waiting_answer_timeout_.isActive() && 
		(state_ == PAChatAIState_Done || state_ == PAChatAIState_Failed);
}

QString PAChatAI::GetQuestion(size_t index)
{
	QStringList& questions = std::get<0>(questions_answers[index]);
	QStringList& adders = std::get<2>(questions_answers[index]);
	return questions[qrand() % questions.size()] + adders[qrand() % adders.size()];
}

QString PAChatAI::GetReaction(size_t index)
{
	QStringList& reactions = std::get<1>(questions_answers[index]);
	QStringList& adders = std::get<3>(questions_answers[index]);
	return reactions[qrand() % reactions.size()] + adders[qrand() % adders.size()];
}

bool PAChatAI::IsPossibleAnswer(size_t index, QString message)
{
	return std::get<4>(questions_answers[index])(message);
}

bool PAChatAI::IsGoodAnswer(size_t index, QString message, bool* did_extract_age, int* extracted_age)
{
	return std::get<5>(questions_answers[index])(message, did_extract_age, extracted_age);
}

void PAChatAI::AskNextQuestion()
{
	state_ = PAChatAIState_AskingQuestion;
	reaction_sender_.stop();
	int time = std::get<6>(questions_answers[current_index_]);
	question_akser_.start(time);
}

void PAChatAI::PushNextReaction()
{
	state_ = PAChatAIState_SendingReaction;
	question_akser_.stop();
	int time = std::get<7>(questions_answers[current_index_]);
	reaction_sender_.start(time);
}


void PAChatAI::ProcessMessage(QString message)
{
	if (state_ == PAChatAIState_Done || state_ == PAChatAIState_Failed)
	{
		return;
	}

	message = message.toLower();

	if (++amount_incomming_messages_ < 4)
	{
		if (filter_->IsMessageFilteredAI(message))
		{
			PerformFail();
			return;
		}
	}

	if (!got_age_)
	{
		int got_age_count = 0;
		IsGoodAnswer(0, message, &got_age_, &got_age_count);
		if (got_age_)
		{
			if (got_age_count < 16 || got_age_count > 20)
			{
				PerformFail();
				return;
			}
		}
	}

	if (state_ != PAChatAIState_WaitingForAnswer && current_index_ != 0) // skip if first question (asking gender, mostly they do tell it themselves)
	{
		return;
	}

	if (IsPossibleAnswer(current_index_, message))
	{
		if (IsGoodAnswer(current_index_, message, &got_age_))
		{
			waiting_answer_timeout_.stop();
			PushNextReaction();
		}
		else
		{
			PerformFail();
			return;
		}
	}
}

void PAChatAI::onAskNextQuestion()
{
	question_akser_.stop();
	if (got_age_ && current_index_ == 1)
	{
		++current_index_;
	}
	emit onRequestMessage(GetQuestion(current_index_), false);
	state_ = PAChatAIState_WaitingForAnswer;
	waiting_answer_timeout_.start(60000);
}

void PAChatAI::onReactionToAnswer()
{
	reaction_sender_.stop();

	QString reaction = GetReaction(current_index_);
	bool not_last = ++current_index_ < questions_answers.size();

	emit onRequestMessage(reaction, !not_last);

	if (not_last)
	{
		if (got_age_ && current_index_ == 1)
		{
			++current_index_;
		}
		AskNextQuestion();
	}
	else
	{
		Stop();
	}
}

void PAChatAI::onWaitForAnswerTimeout()
{
	PerformFail();
}

void PAChatAI::PerformFail()
{
	Stop();
	state_ = PAChatAIState_Failed;
	emit requestNextChat();
}