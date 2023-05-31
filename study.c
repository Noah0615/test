#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>


#define DATAFILE "/var/chance.data" // 사용자 데이터를 저장할 파일

// 사용자 정보를 저장할 user 구조체
struct user {
	int uid;
	int credits;
	int highscore;
	char name[100];
	int (*current_game) ();
};

// 함수 프로토타입
int get_player_data();
void register_new_player();
void update_player_data();
void show_highscore();
void jackpot();
void input_name();
void print_cards(char*, char*, int);
int take_wager(int, int);
void play_the_game();
int pick_a_number();
int dealer_no_match();
int find_the_ace();
void fatal(char*);

//전역 변수
struct user player; // 플레이어 구조체

int main() {
	int choice, last_game;

	srand(time(0)); // 현재 시작으로 시드


	if (get_player_data() == -1) // 플레이어 데이터 파일 읽기
		register_new_player(); // 파일에 데이터가 없으면 새 플레이어 등록

	while (choice != 7) {
		printf("-=[확률 게임 메뉴 ]=-\n");
		printf("1 - 숫자 뽑기 게임하기\n");
		printf("2 - No Match Dealer 게임하기\n");
		printf("3 - 에이스카드 찾기 게임하기\n");
		printf("4 - 현재 최고 점수 보기\n");
		printf("5 - 사용자 이름 변경\n");
		printf("6 - 100 크레딧으로 리셋하기\n");
		printf("7 - 종료\n");
		printf("[이름: %s]\n", player.name);
		printf("[%u 크레딧이 있습니다] -> ", player.credits);
		scanf("%d", &choice);

		if ((choice < 1) || (choice > 7))
			printf("\n[!!] %d는 잘못된 선택입니다. 다시 선택해주세요.\n\n", choice);
		else if (choice < 4) {     //아니면 게임이 선택된 것임
			if (choice != last_game) { // 함수 포인터가 설정돼 있지 않으면
				if (choice == 1)   // 선택한 게임을 가리키게 설정
					player.current_game = pick_a_number;
				else if (choice == 2)
					player.current_game = dealer_no_match;
				else
					player.current_game = find_the_ace;
				last_game = choice;  // last_game 설정
			}
			play_the_game();   // 게임하기
		}
		else if (choice == 4)
			show_highscore();
		else if (choice == 5) {
			printf("\n사용자 이름 변경\n");
			printf("새로운 이름: ");
			input_name();
			printf("이름이 변경됐습니다.\n\n");
		}
		else if (choice == 6) {
			printf("\n100 크래딧이 됐습니다.\n\n");
			player.credits = 100;
		}
	}
	update_player_data();
	printf("\n게임해주셔서 감사합니다.! 그럼 안녕히.\n");
}
		

// 이 함수는 현재 uid의 플레이어 데이터를 파일에서 읽어온다.
// 현재 uid의 플레이어 데이터를 찾을 수 없으면 -1 리턴
int get_player_data() {
	int fd, uid, read_bytes;
	struct user entry;

	uid = getuid();

	fd = open(DATAFILE, O_RDONLY);
	if (fd == -1) // 파일을 열 수 없음. 파일이 존재하지 않음
		return -1;
	read_bytes = read(fd, &entry, sizeof(struct user)); // 파일 첫 부분 읽기
	while (entry.uid != uid && read_bytes > 0) { // 맞는 uid를 찾을 때까지 반복
		read_bytes = read(fd, &entry, sizeof(struct user)); // 계속 읽기
	}
	close(fd);

	if (read_bytes < sizeof(struct user)) // 파일 끝까지 읽었음을 의미함
		return -1;
	else
		player = entry; // 읽어온 값을 플레이어 구조체에 복사
	return 1;		// 성공
}

// 새 사용자 등록 함수
// 새로운 플레이어 계정을 생성하고 파일 뒤에 추가
void register_new_player() {
	int fd;

	printf("-=-={ 새 플레이어 등록 }=-=-\n");
	printf("이름: ");
	input_name();

	player.uid = getuid();
	player.highscore = player.credits = 100;

	fd = open(DATAFILE, O_WRONLY|O_CREAT|O_APPEND);
	if (fd == -1)
		fatal("register_new_player()에서 파일 열던 중");
	write(fd, &player, sizeof(struct user));
	close(fd);

	printf("\n%s님 확률 게임에 온 것을 환영합니다.\n", player.name);
	printf("%u 크레딧이 있습니다.\n", player.credits);
}

// 현재 플레이어 데이터를 파일에 쓰는 함수
// 게임이 끝난 후 크레딧을 업데이트하는 데 주로 이용됨

void update_player_data() {
	int fd, i, read_uid;
	char burned_byte;

	fd = open(DATAFILE, O_RDWR);
	if (fd == -1) // 여기서 파일 열기에 실패한다면 큰 문제임
		fatal("update_player_data()에서 파일 열던 중");
	read(fd, &read_uid, 4);  // 첫 번째 구조체에서 uid 읽기
	while (read_uid != player.uid) { // 맞는 uid를 찾을 때까지 반복
		for (i = 0; i < sizeof(struct user) - 4; i++) // 구조체 나머지에서 읽기
			read(fd, &burned_byte, 1);
		read(fd, &read_uid, 4);
	}
	write(fd, &(player.credits), 4);  // 크레딧 업데이트
	write(fd, &(player.highscore), 4); // 최고 점수 업데이트
	write(fd, &(player.name), 100);  // 이름 업데이트
	close(fd);
}

// 현재 최고 점수와 최고득점자를 보여주는 함수.
void show_highscore() {
	unsigned int top_score = 0;
	char top_name[100];
	struct user entry;
	int fd;

	printf("\n===============| 최고 점수 |=================\n");
	fd = open(DATAFILE, O_RDONLY);
	if (fd == -1)
		fatal("show_highscore()에서 파일 열던 중");
	while (read(fd, &entry, sizeof(struct user)) > 0) { //파일 끝까지 반복
		if (entry.highscore > top_score) { // 최고 점수가 있으면
			top_score = entry.highscore;  // 그 점수로 top_score 설정
			strcpy(top_name, entry.name); // 그 이름으로 top_name 설정
		}
	}
	close(fd);
	if (top_score > player.highscore)
		printf("%s이(가) 최고 점수 %u를 기록했습니다.\n", top_name, top_score);
	else
		printf("이제 당신이 %u로 최고 점수를 기록합니다!\n", player.highscore);

	printf("=========================================================================\n\n");
}

//숫자 뽑기 게임의 잭팟 함수
void jackpot() {
	printf("*+*+*+*+*+*+*+*+ 잭팟 *+*+*+*+*+*+*+*+\n");
	printf("100 크레딧의 잭팟에 당첨됐습니다!\n");
	player.credits += 100;
}

// 플레이어 이름을 입력하는 데 사용하는 함수
// scanf("%s", &whatever)는 공백이 있으면 동작하지 않으므로 이 함수가 필요함
void input_name() {
	char* name_ptr, input_char = '\n';
	while (input_char == '\n') // 남아있는 것은 지워버림
		scanf("%c", &input_char);

	name_ptr = (char*)&(player.name); // name_ptr = player name의 주소
	while (input_char != '\n') { // 새 줄까지 반복
		*name_ptr = input_char; // 입력 문자를 이름 값에 넣음
		scanf("%c", &input_char);
		name_ptr++;
	}
	*name_ptr = 0; // 문자열 끝냄
}

//에이스카드 찾기 게임용 세 장의 카드 출력 함수
//출력할 메시지와 카드 배열 포인터와 사용자가 선택한 카드를 입력 인자로 받는다.
// user_pick이 -1이면 카드 선택 번호를 보여준다.
void print_cards(char* message, char* cards, int user_pick) {
	int i;

	printf("\n\t*** %s ***\n", message);
	printf("     \t._.\t._.\t._.\n");
	printf("카드:\t|%c|\t|%c|\t|%c|\n\t", cards[0], cards[1], cards[2]);
	if (user_pick == -1)
		printf(" 1 \t 2 \t 3\n");
	else {
		for (i = 0; i < user_pick; i++)
			printf("\t");
		printf(" ^-- 당신의 선택\n");
	}
}

// No Match Dealer 게임과 에이스카드 찾기 게임을 위한 베팅하는 함수
// 가능한 크레딧과 이전 베팅을 인자로 받는다.
// 이전 베팅 previous_wager는 에이스카드 찾기 게임에서 두 번째 베팅에서 
// 아주 중요하다.
// 베팅액이 너무 크거나 너무 적으면 -1 리턴
// 아니면 베팅액을 리턴
int take_wager(int available_credits, int previous_wager) {
	int wager, total_wager;

	printf("%d 크레딧 중 얼마나 거시겠습니까? ", available_credits);
	scanf("%d", &wager);
	if (wager < 1) { // 베팅액은 0을 초과해야 함
		printf("양수 값으로 배팅해야 합니다!\n");
		return -1;
	}
	total_wager = previous_wager + wager;
	if (total_wager > available_credits) {  // 가능한 크레딧 확인
		printf("Your total wager of %d is more than you haver!\n", total_wager);
		printf("You only have %d available credits, try again.\n", available_credits);
		return -1;
	}
	return wager;
}

// 현재 게임을 다시 하게 허용하는 루프가 있다.
// 각 게임 후에 새 크레딧 총합을 파일에 쓴다.
void play_the_game() {
	int play_again = 1;
	int(*game) ();
	char selection;
	while (play_again) {
		printf("\n[DEBUG] current_game 포인터 @ 0x%08x\n", player.current_game);
		if (player.current_game() != -1) { // 오류 없이 게임이 진행되고
			if (player.credits > player.highscore) // 새 최고 점수가 기록되면
				player.highscore = player.credits; // 최고 점수 업데이트
			printf("\n이제 %u 크레딧이 있습니다.\n", player.credits);
			update_player_data();    // 새 크레딧 총합을 파일에 씀
			printf("다시 하시겠습니까? (y/n)   ");
			selection = '\n';
			while (selection == '\n')           // 남은 줄은 지워버림
				scanf("%c", &selection);
			if (selection == 'n')
				play_again = 0;
		}
		else       // 오류가 발생하면 메인 메뉴로 돌아감
			play_again = 0;
	}
}

// 숫자 뽑기 게임
// 플레이어의 크레딧이 충분치 않으면 -1 리턴
int pick_a_number() {
	int pick, winning_number;

	printf("\n########## 숫자 뽑기 ##########\n");
	printf("게임하는데 10 크레딧이 필요합니다. 단지 1부터 20 사이의\n");
	printf("숫자 하나를 선택하면 됩니다. 올바른 숫자를 선택하면\n");
	printf("잭팟에 당첨돼 100 크레딧을 받을 수 있습니다!\n\n");
	winning_number = (rand() % 20) + 1; // 1부터 20 사이의 숫자 선택
	if (player.credits < 10) {
		printf("%d 크레딧밖에 없습니다. 게임하는 데 충분치 않습니다!\n\n", player.credits);
		return -1; // 게임할 크레딧이 부족함
	}
	player.credits -= 10;  // 10 크레딧 감소
	printf("10 크레딧을 사용했습니다.\n");
	printf("1부터 20 사이의 숫자 뽑기: ");
	scanf("%d", &pick);

	printf("컴퓨터가 선택한 숫자는 %d\n", winning_number);
	if (pick == winning_number)
		jackpot();
	else
		printf("죄송합니다. 틀렸습니다.\n");
	return 0;
}

// No Match Dealer 게임
// 플레이어의 크레딧이 0이 되면 -1 리턴
int dealer_no_match() {
	int i, j, numbers[16], wager = -1, match = -1;

	printf("\n:::::: No Match Dealer ::::::\n");
	printf("이 게임에서는 당신의 모든 크레딧을 베팅할 수도 있습니다.\n");
	printf("딜러는 0부터 99사이의 16개의 임의의 수를 뽑을 것입니다.\n");
	printf("16개의 수 사이에 같은 수가 없으면 당신은 2배를 따게 됩니다!\n\n");


	if (player.credits == 0) {
		printf("베팅을 해야 합니다!\n\n");
		return -1;
	}
	while (wager == -1)
		wager = take_wager(player.credits, 0);

	printf("\t\t::: 16개의 임의 수 선택 :::\n");
	for (i = 0; i < 16; i++) {
		numbers[i] = rand() % 10; // 0부터 99사이의 숫자 선택
		printf("%2d\t", numbers[i]);
		if (i % 8 == 7)		// 8개 숫자를 한 줄로 끊어서 출력
			printf("\n");
	}
	for (i = 0; i < 15; i++) {		//같은 수가 있는지 살펴봄
		j = i + 1;
		while (j < 16) {
			if (numbers[i] == numbers[j])
				match = numbers[i];
			j++;
		}
	}
	if (match != -1) {
		printf("딜러가 뽑은 숫자 중 %d가 일치합니다!\n", match);
		printf("%d 크레딧을 잃습니다.\n", wager);
		player.credits -= wager;
	}
	else {
		printf("같은 수가 없습니다! %d 크레딧을 땁니다!\n", wager);
		player.credits += wager;
	}
	return 0;
}

//에이스카드 찾기 게임
// 플레이어의 크레딧이 0이 되면 -1 리턴
int find_the_ace() {
	int i, ace, total_wager;
	int invalid_choice, pick = -1, wager_one = -1, wager_two = -1;
	char choice_two, cards[3] = { 'X', 'X', 'X' };

	ace = rand() % 3; // 에이스카드를 무작위로 선택

	printf("********** 에이스카드 찾기 **********");
	printf("이 게임에서는 당신의 모든 크레딧을 베팅할 수도 있습니다. \n");
	printf("세 장의 카드, 두 장의 퀸과 한 장의 에이스카드가 있습니다.\n");
	printf("에이스카드를 찾으면 베팅액을 따게 됩니다.\n");
	printf("한 장의 카드를 선택한 후 한 장의 퀸 카드를 보여줍니다.\n");
	printf("이때 당신은 선택을 바꿀 수도 있고 유지하면서 베팅을 올릴 수도 있습니다.\n");


	if (player.credits == 0) {
		printf("베팅을 해야 합니다!\n\n");
		return -1;
	}

	while (wager_one == -1)  // 올바른 베팅을 할 때까지 반복 
		wager_one = take_wager(player.credits, 0);

	print_cards("카드 분배", cards, -1);
	pick = -1;
	while ((pick < 1) || (pick > 3)) {	//올바른 선택을 할 때까지 반복
		printf("카드를 선택하세요: 1, 2, 3:  ");
		scanf("%d", &pick);
	}
	pick--; // 카드 번호는 0부터 시작하므로 1빼서 조정
	i = 0;
	while (i == ace || i == pick) //  한 장 보여줄 퀸 카드를 찾을 때까지 반복
		i++;
	cards[i] = 'Q';
	print_cards("퀸 카드 보여주기", cards, pick);
	invalid_choice = 1;
	while (invalid_choice) {      // 올바른 선택을 할 때까지 반복
		printf("[c]선택을 바꾸겠습니까\t아니면\t[i]를 유지하고 베팅을 올리시겠습니까?\n");
		printf("c와 i 중 선택하세요:  ");
		choice_two = '\n';
		while (choice_two == '\n')   //  남은 줄은 지워버린다
			scanf("%c", &choice_two);
		if (choice_two == 'i') {    // 베팅 증액
			invalid_choice = 0;  // 올바른 선택
			while (wager_two == -1) // 올바른 두 번째 베팅을 할 때까지 반복
				wager_two = take_wager(player.credits, wager_one);
		}
		if (choice_two == 'c') { // 선택 변경
			i = invalid_choice = 0; // 올바른 선택
			while (i == pick || cards[i] == 'Q') // 다른 카드를 찾고 선택을 바꿀 때까지 반복
				i++;
			pick = i;
			printf("%d 카드로 선택을 변경했습니다.\n", pick + 1);
		}
	}
	for (i = 0; i < 3; i++) {
		if (ace == i)
			cards[i] = 'A';
		else
			cards[i] = 'Q';
	}
	print_cards("결과", cards, pick);

	if (pick == ace) {	//승리
		printf("첫 번째 베팅으로 %d 크레딧을 땄습니다!\n", wager_one);
		player.credits += wager_one;
		if (wager_two != -1) {
			printf("그리고 두 번째 베팅으로 추가 %d 크레딧을 땄습니다!\n", wager_two);
			player.credits += wager_two;
		}
	}
	else {			//패배
		printf("첫 번째 베팅으로 %d 크레딧을 잃었습니다!\n", wager_one);
		player.credits -= wager_one;
		if (wager_two != -1) {
			printf("그리고 두 번째 베팅으로 추가 %d 크레딧을 잃었습니다!\n", wager_two);
			player.credits -= wager_two;
		}
	}

		return 0;
}
