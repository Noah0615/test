#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>


#define DATAFILE "/var/chance.data" // ����� �����͸� ������ ����

// ����� ������ ������ user ����ü
struct user {
	int uid;
	int credits;
	int highscore;
	char name[100];
	int (*current_game) ();
};

// �Լ� ������Ÿ��
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

//���� ����
struct user player; // �÷��̾� ����ü

int main() {
	int choice, last_game;

	srand(time(0)); // ���� �������� �õ�


	if (get_player_data() == -1) // �÷��̾� ������ ���� �б�
		register_new_player(); // ���Ͽ� �����Ͱ� ������ �� �÷��̾� ���

	while (choice != 7) {
		printf("-=[Ȯ�� ���� �޴� ]=-\n");
		printf("1 - ���� �̱� �����ϱ�\n");
		printf("2 - No Match Dealer �����ϱ�\n");
		printf("3 - ���̽�ī�� ã�� �����ϱ�\n");
		printf("4 - ���� �ְ� ���� ����\n");
		printf("5 - ����� �̸� ����\n");
		printf("6 - 100 ũ�������� �����ϱ�\n");
		printf("7 - ����\n");
		printf("[�̸�: %s]\n", player.name);
		printf("[%u ũ������ �ֽ��ϴ�] -> ", player.credits);
		scanf("%d", &choice);

		if ((choice < 1) || (choice > 7))
			printf("\n[!!] %d�� �߸��� �����Դϴ�. �ٽ� �������ּ���.\n\n", choice);
		else if (choice < 4) {     //�ƴϸ� ������ ���õ� ����
			if (choice != last_game) { // �Լ� �����Ͱ� ������ ���� ������
				if (choice == 1)   // ������ ������ ����Ű�� ����
					player.current_game = pick_a_number;
				else if (choice == 2)
					player.current_game = dealer_no_match;
				else
					player.current_game = find_the_ace;
				last_game = choice;  // last_game ����
			}
			play_the_game();   // �����ϱ�
		}
		else if (choice == 4)
			show_highscore();
		else if (choice == 5) {
			printf("\n����� �̸� ����\n");
			printf("���ο� �̸�: ");
			input_name();
			printf("�̸��� ����ƽ��ϴ�.\n\n");
		}
		else if (choice == 6) {
			printf("\n100 ũ������ �ƽ��ϴ�.\n\n");
			player.credits = 100;
		}
	}
	update_player_data();
	printf("\n�������ּż� �����մϴ�.! �׷� �ȳ���.\n");
}
		

// �� �Լ��� ���� uid�� �÷��̾� �����͸� ���Ͽ��� �о�´�.
// ���� uid�� �÷��̾� �����͸� ã�� �� ������ -1 ����
int get_player_data() {
	int fd, uid, read_bytes;
	struct user entry;

	uid = getuid();

	fd = open(DATAFILE, O_RDONLY);
	if (fd == -1) // ������ �� �� ����. ������ �������� ����
		return -1;
	read_bytes = read(fd, &entry, sizeof(struct user)); // ���� ù �κ� �б�
	while (entry.uid != uid && read_bytes > 0) { // �´� uid�� ã�� ������ �ݺ�
		read_bytes = read(fd, &entry, sizeof(struct user)); // ��� �б�
	}
	close(fd);

	if (read_bytes < sizeof(struct user)) // ���� ������ �о����� �ǹ���
		return -1;
	else
		player = entry; // �о�� ���� �÷��̾� ����ü�� ����
	return 1;		// ����
}

// �� ����� ��� �Լ�
// ���ο� �÷��̾� ������ �����ϰ� ���� �ڿ� �߰�
void register_new_player() {
	int fd;

	printf("-=-={ �� �÷��̾� ��� }=-=-\n");
	printf("�̸�: ");
	input_name();

	player.uid = getuid();
	player.highscore = player.credits = 100;

	fd = open(DATAFILE, O_WRONLY|O_CREAT|O_APPEND);
	if (fd == -1)
		fatal("register_new_player()���� ���� ���� ��");
	write(fd, &player, sizeof(struct user));
	close(fd);

	printf("\n%s�� Ȯ�� ���ӿ� �� ���� ȯ���մϴ�.\n", player.name);
	printf("%u ũ������ �ֽ��ϴ�.\n", player.credits);
}

// ���� �÷��̾� �����͸� ���Ͽ� ���� �Լ�
// ������ ���� �� ũ������ ������Ʈ�ϴ� �� �ַ� �̿��

void update_player_data() {
	int fd, i, read_uid;
	char burned_byte;

	fd = open(DATAFILE, O_RDWR);
	if (fd == -1) // ���⼭ ���� ���⿡ �����Ѵٸ� ū ������
		fatal("update_player_data()���� ���� ���� ��");
	read(fd, &read_uid, 4);  // ù ��° ����ü���� uid �б�
	while (read_uid != player.uid) { // �´� uid�� ã�� ������ �ݺ�
		for (i = 0; i < sizeof(struct user) - 4; i++) // ����ü ���������� �б�
			read(fd, &burned_byte, 1);
		read(fd, &read_uid, 4);
	}
	write(fd, &(player.credits), 4);  // ũ���� ������Ʈ
	write(fd, &(player.highscore), 4); // �ְ� ���� ������Ʈ
	write(fd, &(player.name), 100);  // �̸� ������Ʈ
	close(fd);
}

// ���� �ְ� ������ �ְ�����ڸ� �����ִ� �Լ�.
void show_highscore() {
	unsigned int top_score = 0;
	char top_name[100];
	struct user entry;
	int fd;

	printf("\n===============| �ְ� ���� |=================\n");
	fd = open(DATAFILE, O_RDONLY);
	if (fd == -1)
		fatal("show_highscore()���� ���� ���� ��");
	while (read(fd, &entry, sizeof(struct user)) > 0) { //���� ������ �ݺ�
		if (entry.highscore > top_score) { // �ְ� ������ ������
			top_score = entry.highscore;  // �� ������ top_score ����
			strcpy(top_name, entry.name); // �� �̸����� top_name ����
		}
	}
	close(fd);
	if (top_score > player.highscore)
		printf("%s��(��) �ְ� ���� %u�� ����߽��ϴ�.\n", top_name, top_score);
	else
		printf("���� ����� %u�� �ְ� ������ ����մϴ�!\n", player.highscore);

	printf("=========================================================================\n\n");
}

//���� �̱� ������ ���� �Լ�
void jackpot() {
	printf("*+*+*+*+*+*+*+*+ ���� *+*+*+*+*+*+*+*+\n");
	printf("100 ũ������ ���̿� ��÷�ƽ��ϴ�!\n");
	player.credits += 100;
}

// �÷��̾� �̸��� �Է��ϴ� �� ����ϴ� �Լ�
// scanf("%s", &whatever)�� ������ ������ �������� �����Ƿ� �� �Լ��� �ʿ���
void input_name() {
	char* name_ptr, input_char = '\n';
	while (input_char == '\n') // �����ִ� ���� ��������
		scanf("%c", &input_char);

	name_ptr = (char*)&(player.name); // name_ptr = player name�� �ּ�
	while (input_char != '\n') { // �� �ٱ��� �ݺ�
		*name_ptr = input_char; // �Է� ���ڸ� �̸� ���� ����
		scanf("%c", &input_char);
		name_ptr++;
	}
	*name_ptr = 0; // ���ڿ� ����
}

//���̽�ī�� ã�� ���ӿ� �� ���� ī�� ��� �Լ�
//����� �޽����� ī�� �迭 �����Ϳ� ����ڰ� ������ ī�带 �Է� ���ڷ� �޴´�.
// user_pick�� -1�̸� ī�� ���� ��ȣ�� �����ش�.
void print_cards(char* message, char* cards, int user_pick) {
	int i;

	printf("\n\t*** %s ***\n", message);
	printf("     \t._.\t._.\t._.\n");
	printf("ī��:\t|%c|\t|%c|\t|%c|\n\t", cards[0], cards[1], cards[2]);
	if (user_pick == -1)
		printf(" 1 \t 2 \t 3\n");
	else {
		for (i = 0; i < user_pick; i++)
			printf("\t");
		printf(" ^-- ����� ����\n");
	}
}

// No Match Dealer ���Ӱ� ���̽�ī�� ã�� ������ ���� �����ϴ� �Լ�
// ������ ũ������ ���� ������ ���ڷ� �޴´�.
// ���� ���� previous_wager�� ���̽�ī�� ã�� ���ӿ��� �� ��° ���ÿ��� 
// ���� �߿��ϴ�.
// ���þ��� �ʹ� ũ�ų� �ʹ� ������ -1 ����
// �ƴϸ� ���þ��� ����
int take_wager(int available_credits, int previous_wager) {
	int wager, total_wager;

	printf("%d ũ���� �� �󸶳� �Žðڽ��ϱ�? ", available_credits);
	scanf("%d", &wager);
	if (wager < 1) { // ���þ��� 0�� �ʰ��ؾ� ��
		printf("��� ������ �����ؾ� �մϴ�!\n");
		return -1;
	}
	total_wager = previous_wager + wager;
	if (total_wager > available_credits) {  // ������ ũ���� Ȯ��
		printf("Your total wager of %d is more than you haver!\n", total_wager);
		printf("You only have %d available credits, try again.\n", available_credits);
		return -1;
	}
	return wager;
}

// ���� ������ �ٽ� �ϰ� ����ϴ� ������ �ִ�.
// �� ���� �Ŀ� �� ũ���� ������ ���Ͽ� ����.
void play_the_game() {
	int play_again = 1;
	int(*game) ();
	char selection;
	while (play_again) {
		printf("\n[DEBUG] current_game ������ @ 0x%08x\n", player.current_game);
		if (player.current_game() != -1) { // ���� ���� ������ ����ǰ�
			if (player.credits > player.highscore) // �� �ְ� ������ ��ϵǸ�
				player.highscore = player.credits; // �ְ� ���� ������Ʈ
			printf("\n���� %u ũ������ �ֽ��ϴ�.\n", player.credits);
			update_player_data();    // �� ũ���� ������ ���Ͽ� ��
			printf("�ٽ� �Ͻðڽ��ϱ�? (y/n)   ");
			selection = '\n';
			while (selection == '\n')           // ���� ���� ��������
				scanf("%c", &selection);
			if (selection == 'n')
				play_again = 0;
		}
		else       // ������ �߻��ϸ� ���� �޴��� ���ư�
			play_again = 0;
	}
}

// ���� �̱� ����
// �÷��̾��� ũ������ ���ġ ������ -1 ����
int pick_a_number() {
	int pick, winning_number;

	printf("\n########## ���� �̱� ##########\n");
	printf("�����ϴµ� 10 ũ������ �ʿ��մϴ�. ���� 1���� 20 ������\n");
	printf("���� �ϳ��� �����ϸ� �˴ϴ�. �ùٸ� ���ڸ� �����ϸ�\n");
	printf("���̿� ��÷�� 100 ũ������ ���� �� �ֽ��ϴ�!\n\n");
	winning_number = (rand() % 20) + 1; // 1���� 20 ������ ���� ����
	if (player.credits < 10) {
		printf("%d ũ�����ۿ� �����ϴ�. �����ϴ� �� ���ġ �ʽ��ϴ�!\n\n", player.credits);
		return -1; // ������ ũ������ ������
	}
	player.credits -= 10;  // 10 ũ���� ����
	printf("10 ũ������ ����߽��ϴ�.\n");
	printf("1���� 20 ������ ���� �̱�: ");
	scanf("%d", &pick);

	printf("��ǻ�Ͱ� ������ ���ڴ� %d\n", winning_number);
	if (pick == winning_number)
		jackpot();
	else
		printf("�˼��մϴ�. Ʋ�Ƚ��ϴ�.\n");
	return 0;
}

// No Match Dealer ����
// �÷��̾��� ũ������ 0�� �Ǹ� -1 ����
int dealer_no_match() {
	int i, j, numbers[16], wager = -1, match = -1;

	printf("\n:::::: No Match Dealer ::::::\n");
	printf("�� ���ӿ����� ����� ��� ũ������ ������ ���� �ֽ��ϴ�.\n");
	printf("������ 0���� 99������ 16���� ������ ���� ���� ���Դϴ�.\n");
	printf("16���� �� ���̿� ���� ���� ������ ����� 2�踦 ���� �˴ϴ�!\n\n");


	if (player.credits == 0) {
		printf("������ �ؾ� �մϴ�!\n\n");
		return -1;
	}
	while (wager == -1)
		wager = take_wager(player.credits, 0);

	printf("\t\t::: 16���� ���� �� ���� :::\n");
	for (i = 0; i < 16; i++) {
		numbers[i] = rand() % 10; // 0���� 99������ ���� ����
		printf("%2d\t", numbers[i]);
		if (i % 8 == 7)		// 8�� ���ڸ� �� �ٷ� ��� ���
			printf("\n");
	}
	for (i = 0; i < 15; i++) {		//���� ���� �ִ��� ���캽
		j = i + 1;
		while (j < 16) {
			if (numbers[i] == numbers[j])
				match = numbers[i];
			j++;
		}
	}
	if (match != -1) {
		printf("������ ���� ���� �� %d�� ��ġ�մϴ�!\n", match);
		printf("%d ũ������ �ҽ��ϴ�.\n", wager);
		player.credits -= wager;
	}
	else {
		printf("���� ���� �����ϴ�! %d ũ������ ���ϴ�!\n", wager);
		player.credits += wager;
	}
	return 0;
}

//���̽�ī�� ã�� ����
// �÷��̾��� ũ������ 0�� �Ǹ� -1 ����
int find_the_ace() {
	int i, ace, total_wager;
	int invalid_choice, pick = -1, wager_one = -1, wager_two = -1;
	char choice_two, cards[3] = { 'X', 'X', 'X' };

	ace = rand() % 3; // ���̽�ī�带 �������� ����

	printf("********** ���̽�ī�� ã�� **********");
	printf("�� ���ӿ����� ����� ��� ũ������ ������ ���� �ֽ��ϴ�. \n");
	printf("�� ���� ī��, �� ���� ���� �� ���� ���̽�ī�尡 �ֽ��ϴ�.\n");
	printf("���̽�ī�带 ã���� ���þ��� ���� �˴ϴ�.\n");
	printf("�� ���� ī�带 ������ �� �� ���� �� ī�带 �����ݴϴ�.\n");
	printf("�̶� ����� ������ �ٲ� ���� �ְ� �����ϸ鼭 ������ �ø� ���� �ֽ��ϴ�.\n");


	if (player.credits == 0) {
		printf("������ �ؾ� �մϴ�!\n\n");
		return -1;
	}

	while (wager_one == -1)  // �ùٸ� ������ �� ������ �ݺ� 
		wager_one = take_wager(player.credits, 0);

	print_cards("ī�� �й�", cards, -1);
	pick = -1;
	while ((pick < 1) || (pick > 3)) {	//�ùٸ� ������ �� ������ �ݺ�
		printf("ī�带 �����ϼ���: 1, 2, 3:  ");
		scanf("%d", &pick);
	}
	pick--; // ī�� ��ȣ�� 0���� �����ϹǷ� 1���� ����
	i = 0;
	while (i == ace || i == pick) //  �� �� ������ �� ī�带 ã�� ������ �ݺ�
		i++;
	cards[i] = 'Q';
	print_cards("�� ī�� �����ֱ�", cards, pick);
	invalid_choice = 1;
	while (invalid_choice) {      // �ùٸ� ������ �� ������ �ݺ�
		printf("[c]������ �ٲٰڽ��ϱ�\t�ƴϸ�\t[i]�� �����ϰ� ������ �ø��ðڽ��ϱ�?\n");
		printf("c�� i �� �����ϼ���:  ");
		choice_two = '\n';
		while (choice_two == '\n')   //  ���� ���� ����������
			scanf("%c", &choice_two);
		if (choice_two == 'i') {    // ���� ����
			invalid_choice = 0;  // �ùٸ� ����
			while (wager_two == -1) // �ùٸ� �� ��° ������ �� ������ �ݺ�
				wager_two = take_wager(player.credits, wager_one);
		}
		if (choice_two == 'c') { // ���� ����
			i = invalid_choice = 0; // �ùٸ� ����
			while (i == pick || cards[i] == 'Q') // �ٸ� ī�带 ã�� ������ �ٲ� ������ �ݺ�
				i++;
			pick = i;
			printf("%d ī��� ������ �����߽��ϴ�.\n", pick + 1);
		}
	}
	for (i = 0; i < 3; i++) {
		if (ace == i)
			cards[i] = 'A';
		else
			cards[i] = 'Q';
	}
	print_cards("���", cards, pick);

	if (pick == ace) {	//�¸�
		printf("ù ��° �������� %d ũ������ �����ϴ�!\n", wager_one);
		player.credits += wager_one;
		if (wager_two != -1) {
			printf("�׸��� �� ��° �������� �߰� %d ũ������ �����ϴ�!\n", wager_two);
			player.credits += wager_two;
		}
	}
	else {			//�й�
		printf("ù ��° �������� %d ũ������ �Ҿ����ϴ�!\n", wager_one);
		player.credits -= wager_one;
		if (wager_two != -1) {
			printf("�׸��� �� ��° �������� �߰� %d ũ������ �Ҿ����ϴ�!\n", wager_two);
			player.credits -= wager_two;
		}
	}

		return 0;
}
